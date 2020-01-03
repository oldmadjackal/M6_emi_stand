/********************************************************************/
/*								    */
/*            МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "МОДЕЛЬ УР"             */
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


#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_ModelGuided.h"

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

     static   RSS_Module_ModelGuided  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_MODEL_GUIDED_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_MODEL_GUIDED_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                         "МОДЕЛЬ УР"	                           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_ModelGuided_instr  RSS_Module_ModelGuided_InstrList[]={

 { "help",      "?",  "#HELP - список доступных команд", 
                       NULL,
                      &RSS_Module_ModelGuided::cHelp   },
 { "info",      "i",  "#INFO - выдать информацию по объекту",
                      " INFO <Имя> \n"
                      "   Выдать основную информацию по объекту в главное окно\n"
                      " INFO/ <Имя> \n"
                      "   Выдать полную информацию по объекту в отдельное окно",
                      &RSS_Module_ModelGuided::cInfo },
 { "pars",      "p",  "#PARS - задание параметров модели в диалоговом режиме", 
                       NULL,
                      &RSS_Module_ModelGuided::cPars   },
 { "mass",      "m",  "#MASS - задание массы НУР (без двигателя)", 
                       NULL,
                      &RSS_Module_ModelGuided::cMass   },
 { "middle",    "md", "#MIDDLE - площадь характеристического сечения", 
                       NULL,
                      &RSS_Module_ModelGuided::cMiddle   },
 { "starttime", "s",  "#STARTTIME - время стартового участка", 
                       NULL,
                      &RSS_Module_ModelGuided::cStartTime   },
 { "log",       "l",  "#LOG - задание файла лога модели", 
                      " LOG <Имя> <Шаблон путу к файлу>\n"
                      "   Шаблон может включать символ подстановку @ для указания места, куда вставляется имя объекта\n",
                      &RSS_Module_ModelGuided::cLog   },
 { "deviation", "dv", "#DEVIATION - задание отклонений параметров от нормы", 
                      " DEVIATION <Имя> <azim> <elev>\n"
                      "   Задание стандартного отклонения по направлению и углу вылета, градусы\n"
                      " DEVIATION/a <Имя> <azim>\n"
                      "   Задание стандартного отклонения по направлению, градусы\n"
                      " DEVIATION/e <Имя> <elev>\n"
                      "   Задание стандартного отклонения по углу вылета, градусы\n",
                      &RSS_Module_ModelGuided::cDeviation  },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_ModelGuided_instr *RSS_Module_ModelGuided::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_ModelGuided::RSS_Module_ModelGuided(void)

{

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="ModelGuided" ;
          category="Unit" ;
         lego_type="Model" ;

        mInstrList=RSS_Module_ModelGuided_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_ModelGuided::~RSS_Module_ModelGuided(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_ModelGuided::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_ModelGuided *unit ;
                    int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_ModelGuided ;
    if(unit==NULL) {
               SEND_ERROR("Секция ModelGuided: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Модель (простая)") ;

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

     int  RSS_Module_ModelGuided::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Простая модель", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_ModelGuided::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MODELGUIDED"
#define  _SECTION_SHRT_NAME   "MODELGUIDED"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object ModelGuided:") ;
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
       if(status)  SEND_ERROR("Секция ModelGuided: Неизвестная команда") ;
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

  int  RSS_Module_ModelGuided::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_ModelGuided_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_ModelGuided::cInfo(char *cmd)

{
                     char  *name ;
     RSS_Unit_ModelGuided  *unit ;
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

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner     %s\r\n" 
                    "Mass      % 5lf\r\n" 
                    "Slideway  % 5lf\r\n" 
                    "\r\n",
                        unit->Name,      unit->Type, 
                        unit->Owner->Name, 
                        unit->mass, unit->start_time 
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
/*		      Реализация инструкции PARS                    */
/*								    */
/*     PARS <Имя комонента>                                         */

  int  RSS_Module_ModelGuided::cPars(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
  RSS_Unit_ModelGuided  *unit ;
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

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: PARS <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Unit_ModelGuided_Pars_dialog, 
                                      (LPARAM)unit                   ) ;
     if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MASS                    */
/*								    */
/*       MASS <Имя> <Масса>                                         */

  int  RSS_Module_ModelGuided::cMass(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelGuided  *unit ;
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

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: MASS <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение массы") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указана масса" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*---------------------------------------------------- Пропись массы */

                 unit->mass=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MIDDLE                  */
/*								    */
/*       MIDDLE <Имя> <Масса>                                       */

  int  RSS_Module_ModelGuided::cMiddle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelGuided  *unit ;
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

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: MIDDLE <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение площади характеристического сечения") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указана площадь характеристического сечения" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*--------------------------------------------------- Пропись данных */

                 unit->s_middle=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции STARTTIME               */
/*								    */
/*       STARTTIME <Имя> <Время разгонного участка>                 */

  int  RSS_Module_ModelGuided::cStartTime(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelGuided  *unit ;
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

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: STARTTIME <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение продолжительности разгонного участка") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указана продолжительность разгонного участка" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*--------------------------------------------------- Пропись данных */

                 unit->start_time=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LOG                     */
/*								    */
/*       LOG <Имя> <Шаблон пути к файлу лога>                       */
/*  Шаблон может включать символ подстановку @ для указания места,  */
/*   куда вставляется имя объекта                                   */

  int  RSS_Module_ModelGuided::cLog(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *path ;
  RSS_Unit_ModelGuided  *unit ;
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
                     path= pars[1] ;   

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: LOG <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

    if(path==NULL) {  SEND_ERROR("Не указана продолжительность разгонного участка") ;
                               return(-1) ;   }

/*--------------------------------------------------- Пропись данных */

         strncpy(unit->log_path, path, sizeof(unit->log_path)) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции DEVIATION                    */
/*								    */
/*   DEVIATION   <Имя> <По направлению> <По углу вылета>            */
/*   DEVIATION/A <Имя> <По направлению>                             */
/*   DEVIATION/E <Имя> <По углу вылета>                             */

  int  RSS_Module_ModelGuided::cDeviation(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelGuided  *unit ;
                   int   a_flag ;            /* Флаг задания СО по направлению */
                   int   e_flag ;            /* Флаг задания СО по углу вылета */
                  char  *error ;
                  char  *end ;
                   int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          a_flag=0 ;
                          e_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )  a_flag=1 ;
                if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )  e_flag=1 ;

                           cmd=end+1 ;
                     }
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

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: SLIDE <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelGuided *)FindUnit(name) ;                /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение параметра") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указаны параметры" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

        if(a_flag) {
                               unit->s_azim=coord[0] ;
                   }
   else if(e_flag) {
                               unit->s_elev=coord[0] ;
                   }
   else            {
                               unit->s_azim=coord[0] ;
              if(coord_cnt>1)  unit->s_elev=coord[1] ;
                   }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     Поиск обьекта типа ModelGuided по имени                */

  RSS_Unit *RSS_Module_ModelGuided::FindUnit(char *name)

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

     if(strcmp(unit->Type, "ModelGuided")) {

           SEND_ERROR("Компонент не является компонентом типа ModelGuided") ;
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
/**            ОПИСАНИЕ КЛАССА КОМПОНЕНТА "МОДЕЛЬ УР"              **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_ModelGuided::RSS_Unit_ModelGuided(void)

{
   strcpy(Type, "ModelGuided") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

              t_0=  0. ; 
             mass=  0. ; 
       start_time=  0. ;
         s_middle=  0. ;
           s_azim=  0. ;
           s_elev=  0. ;
    engine_thrust=  0. ; 
    engine_mass  =  0. ; 

   memset(log_path, 0, sizeof(log_path)) ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_ModelGuided::~RSS_Unit_ModelGuided(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_ModelGuided::vFree(void)

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

    class RSS_Unit *RSS_Unit_ModelGuided::vCopy(RSS_Object *owner)

{
        RSS_Model_data  create_data ;
  RSS_Unit_ModelGuided *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_ModelGuided *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

      strcpy(unit->Name, this->Name) ; 
             unit->Owner=owner ;

    if(owner!=NULL)  owner->Units.Add(unit, "") ;

/*------------------------------------- Копирование настроек объекта */

             unit->mass      =this->mass ;
             unit->s_azim    =this->s_azim ;
             unit->s_elev    =this->s_elev ;
             unit->start_time=this->start_time ;
             unit->s_middle  =this->s_middle ;
      strcpy(unit->log_path,  this->log_path) ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_ModelGuided::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_ModelGuided::vCalculateStart(double  t)
{
        char  error[1024] ;

   const char *log_header="Time;P;V;Nx;Ny;Nz;X;Y;Z;" ;

/*------------------------------------ Инициализация рабочих данных */

    this->t_0          =t ;

    this->engine_thrust=0. ;
    this->engine_mass  =0. ;

/*----------------------------------------- Контроль наличия данных */

    if(this->s_middle<=0.) {
                sprintf(error, "RSS_Unit_ModelGuided::vCalculateStart - Не задана площадь характеристического сечения %s", this->Owner->Name) ;
             SEND_ERROR(error) ;
                   return(-1) ;
                           }

/*-------------------------------------------- Подготовка телеметрии */

                 Log(log_header) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_ModelGuided::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Object *parent ;
      double  V ;
      double  M ;
      double  Fa ;       /* Сила аэродинамического сопротивления */
      double  A_size ;
  RSS_Vector  A ;
      double  dt ;
      double  Vc ;
      double  Cx ;
      double  p ;
        char  error[1024] ;
        char  value[1024] ;
        char  text[2048] ;
         int  i ;

   static struct { double M ; double Cx ; } Cx_table[7]={ { 0.0 , 0.44 },
                                                          { 0.7 , 0.43 },
                                                          { 0.9 , 0.69 },
                                                          { 1.05, 1.18 },
                                                          { 1.2 , 1.36 },
                                                          { 1.5 , 1.00 },
                                                          { 9.0 , 1.00 },
                                                        } ;

#define   _G  9.8

/*------------------------------------------------ Пoдготовка данных */

        if(t1<this->t_0)  return(0) ;

              text[0]=0 ;

           t1-=this->t_0 ;
           t2-=this->t_0 ;
           dt =t2-t1 ;

        parent=this->Owner ;

             V=sqrt(parent->x_velocity*parent->x_velocity+
                    parent->y_velocity*parent->y_velocity+
                    parent->z_velocity*parent->z_velocity ) ;

    if(this->mass+this->engine_mass<=0.) {
                sprintf(error, "RSS_Unit_ModelGuided::vCalculate - Нулевая суммарная масса ракеты %s", parent->Name) ;
             SEND_ERROR(error) ;
                   return(-1) ;
                                         }

       sprintf(value, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;",
                        t1, this->engine_thrust, V,
                            this->control_vector.x, this->control_vector.y, this->control_vector.z,
                            parent->x_base, parent->y_base, parent->z_base) ;
        strcat(text, value) ;

/*----------------------- Приведение управляющей перегрузки к ракете */

/*-------------------- Определение допустимой управляющей перегрузки */

/*------- Приведение управляющей перегрузки к абсолютным координатам */

/*--------------------------------- Определение табличных параметров */

               Vc=330. ;                                            /* Скорость звука */
                p=1.2 ;                                             /* Плотность воздуха */

               M=V/Vc ;

    for(i=1 ; i<100 ; i++)
      if(M<Cx_table[i].M) {  Cx=Cx_table[i-1].Cx ;  break ;  }

/*------------------------------- Определение касательного ускорения */

              Fa= 0.5*Cx*(V/Vc)*p*this->s_middle ;
          A_size=(this->engine_thrust-Fa)/(this->engine_mass+this->mass)
                -_G*sin(parent->a_elev) ; 

/*--------------------------------- Определение суммарного ускорения */

   if(t1>start_time) {
                         A.x =this->control_vector.x ;
                         A.y =this->control_vector.y ;
                         A.z =this->control_vector.z ;
                     }
   else              {
                         A.x =0. ;
                         A.y =0. ;
                         A.z =0. ;
                     }

                         A.x+=A_size*cos(parent->a_elev*_GRD_TO_RAD)*sin(parent->a_azim*_GRD_TO_RAD) ;
                         A.y+=A_size*sin(parent->a_elev*_GRD_TO_RAD) ;
                         A.z+=A_size*cos(parent->a_elev*_GRD_TO_RAD)*cos(parent->a_azim*_GRD_TO_RAD) ;

/*---------------------------------- Определение выходных параметров */


             parent->x_base    +=parent->x_velocity*dt+0.5*A.x*dt*dt ;
             parent->y_base    +=parent->y_velocity*dt+0.5*A.y*dt*dt ;
             parent->z_base    +=parent->z_velocity*dt+0.5*A.z*dt*dt ;

             parent->x_velocity+=A.x*dt ;
             parent->y_velocity+=A.y*dt ;
             parent->z_velocity+=A.z*dt ;

                              V = sqrt(parent->x_velocity*parent->x_velocity+
                                       parent->z_velocity*parent->z_velocity ) ;
             parent->a_elev     =atan2(parent->y_velocity, V)*_RAD_TO_GRD ;
             parent->a_azim     =atan2(parent->x_velocity, parent->z_velocity)*_RAD_TO_GRD ;

/*------------------------------------------------- Сброс телеметрии */

                 Log(text) ;

/*-------------------------------------------------------------------*/

#undef   _G 

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_ModelGuided::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}


/*********************************************************************/
/*								     */
/*              Управление аэродинамическими поверхностями     	     */

    int  RSS_Unit_ModelGuided::vSetAeroControl(RSS_Unit_Aero_Control *aeros, int  aeros_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Тяга двигателя                	     */

    int  RSS_Unit_ModelGuided::vSetEngineThrust(RSS_Unit_Engine_Thrust *thrust, int  thrust_cnt)

{
   this->engine_thrust=thrust->z ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  Масса и положение центра масс двигателя            */

    int  RSS_Unit_ModelGuided::vSetEngineMass(double  mass, RSS_Point *center)

{
   this->engine_mass=mass ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  Моменты инерции двигателя                          */

    int  RSS_Unit_ModelGuided::vSetEngineMI(double  Ix, double  Iy, double  Iz)

{
   return(0) ;
}

/*********************************************************************/
/*								     */
/*                   Требуемая перегрузка маневра                    */

    int  RSS_Unit_ModelGuided::vSetVectorControl(RSS_Vector *vector)

{
    this->control_vector=*vector ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		Запись сообщения в лог-файл			    */	

   void  RSS_Unit_ModelGuided::Log(const char *message)

{
    char  path[FILENAME_MAX] ;
    FILE *file ;
    char *mark ;


    if(this->log_path[0]==0)  return ;
        
       mark=strchr(this->log_path, '@') ;

    if(mark==NULL) {
                      strcpy(path, this->log_path) ;
                   }
    else           {
                      memset(path, 0, sizeof(path)) ;

      if(mark!=this->log_path) 
                     strncpy(path, this->log_path, mark-this->log_path) ;
                      strcat(path, this->Owner->Name) ;
                      strcat(path, mark+1) ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(message, 1, strlen(message), file) ;
           fwrite("\n",    1, strlen("\n"   ), file) ;

           fclose(file) ;
}



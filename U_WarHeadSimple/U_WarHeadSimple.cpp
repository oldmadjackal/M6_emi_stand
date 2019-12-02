/********************************************************************/
/*								    */
/*            МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ БЧ"            */
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

#include "U_WarHeadSimple.h"

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

     static   RSS_Module_WarHeadSimple  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_WARHEAD_SIMPLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_WARHEAD_SIMPLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                         "ПРОСТАЯ БЧ"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_WarHeadSimple_instr  RSS_Module_WarHeadSimple_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_WarHeadSimple::cHelp   },
 { "info",    "i",  "#INFO - выдать информацию по компоненту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_WarHeadSimple::cInfo },
 { "pars",    "p",  "#PARS - задать параметры компонента в диалоговом режиме",
                    " PARS <Имя> \n"
                    "   Задать параметры компонента в диалоговом режиме\n",
                    &RSS_Module_WarHeadSimple::cPars },
 { "tripping","t",  "#TRIPPING - задать настройки исполнительного механизма",
                    " TRIPPING/А <Имя> <Высота срабатывания>\n"
                    "   Установить высоту срабатывания\n"
                    " TRIPPING/T <Имя> <Время срабатывания>\n"
                    "   Установить время срабатывания\n",
                    &RSS_Module_WarHeadSimple::cTripping },
 { "grenade", "g",  "#GRENADE - задать характеристики ОФ-снаряжения",
                    " GRENADE <Имя> <Радиус поражения> <Радиус вспышки>\n"
                    "   Задать характеристики БЧ в ОФ-снаряжении\n",
                    &RSS_Module_WarHeadSimple::cGrenade },
 { "stripe",  "s",  "#STRIPE - задать характеристики кассетной БЧ с линейным выбросом",
                    " STRIPE <Имя> <Суб-боеприпас> <Число суб-боеприпасов> <Шаг выброса>\n"
                    "   Задать характеристики кассетной БЧ с линейным выбросом\n",
                    &RSS_Module_WarHeadSimple::cStripe },
 { "pancakes","pc", "#PANCACES - задать характеристики кассетной БЧ с выбросом последовательными кругами ('блинчики')",
                    " PANCACES <Имя> <Суб-боеприпас> <Число суб-боеприпасов> <Шаг выброса> <Число кругов> <Радиус круга>\n"
                    "   Задать характеристики кассетной БЧ с выбросом последовательными кругами\n",
                    &RSS_Module_WarHeadSimple::cPancakes },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_WarHeadSimple_instr *RSS_Module_WarHeadSimple::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_WarHeadSimple::RSS_Module_WarHeadSimple(void)

{

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="WarHeadSimple" ;
          category="Unit" ;
         lego_type="WarHead" ;

        mInstrList=RSS_Module_WarHeadSimple_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_WarHeadSimple::~RSS_Module_WarHeadSimple(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_WarHeadSimple::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_WarHeadSimple *unit ;
                      int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_WarHeadSimple ;
    if(unit==NULL) {
               SEND_ERROR("Секция WarHeadSimple: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Боевая часть (простой)") ;

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

     int  RSS_Module_WarHeadSimple::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Простая боевая часть", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_WarHeadSimple::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "WARHEADSIMPLE"
#define  _SECTION_SHRT_NAME   "WARHEADSIMPLE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object WarHeadSimple:") ;
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
       if(status)  SEND_ERROR("Секция WarHeadSimple: Неизвестная команда") ;
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

  int  RSS_Module_WarHeadSimple::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_WarHeadSimple_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_WarHeadSimple::cInfo(char *cmd)

{
                     char  *name ;
   RSS_Unit_WarHeadSimple  *unit ;
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

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
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
/*		      Реализация инструкции PARS                    */
/*								    */
/*     PARS <Имя комонента>                                         */

  int  RSS_Module_WarHeadSimple::cPars(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
  RSS_Unit_WarHeadSimple *unit ;
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
                                      "Например: PARS <Имя_компонентa> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_WarHeadSimple_Pars_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TRIPPING                */
/*								    */
/*      TRIPPING/A <Имя> <Высота срабатывания>                      */
/*      TRIPPING/T <Имя> <Время срабатывания>                       */

  int  RSS_Module_WarHeadSimple::cTripping(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
                     int   a_flag ;            /* Флаг срабатывания по высоте */
                     int   t_flag ;            /* Флаг срабатывания по времени */
                    char   *error ;
                    char   *end ;
                     int    i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          a_flag=0 ;
                          t_flag=0 ;

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
                if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  t_flag=1 ;

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

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: TRIPPING <Имя_компонентa> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Контроль вида срабатывания */

    if(a_flag==0 &&
       t_flag==0   ) {
                        SEND_ERROR("Не задан вид срабатывания. \n"
                                   "Необходимо использовать TRIPPING/A или TRIPPING/T") ;
                                         return(-1) ;
                     }

    if(a_flag==1 &&
       t_flag==1   ) {
                        SEND_ERROR("Допустимо задание только одного вида срабатывания. \n"
                                   "Необходимо использовать TRIPPING/A или TRIPPING/T") ;
                                         return(-1) ;
                     }
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
      if(coord_cnt==0)  error="Не указан параметр срабатывания" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

   if(a_flag) {
                 unit->tripping_type    =_BY_ALTITUDE ;
                 unit->tripping_altitude= coord[0] ;
              }

   if(t_flag) {
                 unit->tripping_type    =_BY_TIME ;
                 unit->tripping_time    = coord[0] ;
              }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции GRENADE                 */
/*								    */
/*      GRENADE <Имя> <Радиус поражения> <Радиус вспышки>           */

  int  RSS_Module_WarHeadSimple::cGrenade(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
                    char   *error ;
                    char   *end ;
                     int    i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
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

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: GRENADE <Имя_компонентa> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
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
      if(coord_cnt<2)  error="Не указаны параметры БЧ" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

                 unit-> load_type  =_GRENADE_TYPE ;
                 unit->  hit_range = coord[0] ;
                 unit->blast_radius= coord[1] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции STRIPE                  */
/*								    */
/*   STRIPE <Имя> <Суб-боеприпас> ...                               */
/*                  ... <Число суб-боеприпасов> <Шаг выброса>       */

  int  RSS_Module_WarHeadSimple::cStripe(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char  *sub_name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
              RSS_Object  *sub_object ;
                    char   text[1024] ;
                    char  *error ; 
                    char  *end ;
                     int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                 sub_name= pars[1] ;
                      xyz=&pars[2] ;   

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: STRIPE <Имя_компонентa> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------ Проверка суб-боеприпаса */

    if(sub_name   ==NULL ||
       sub_name[0]==  0    ) {                                      /* Если имя суб-боеприпаса не задано... */
                           SEND_ERROR("Не задано имя суб-боеприпаса. \n"
                                      "Например: STRIPE <Имя компонентa> <Имя суб-боеприпаса> ...") ;
                                         return(-1) ;
                             }

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, sub_name)) {  sub_object=OBJECTS[i] ;
                                                              break ;         }

         if(i>=OBJECTS_CNT) {
                                sprintf(text, "Объект '%s' не найден ", sub_name) ;
                             SEND_ERROR(text) ;
                                  return(NULL) ;
                            }

#undef   OBJECTS
#undef   OBJECTS_CNT

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
      if(coord_cnt<2)  error="Не указаны параметры БЧ" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

                 unit->load_type  =_STRIPE_TYPE ;
         strncpy(unit-> sub_unit, sub_name, sizeof(unit->sub_unit)-1) ;
                 unit-> sub_object= sub_object ;
                 unit-> sub_count = (int)coord[0] ;
                 unit-> sub_step  =      coord[1] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PANCAKES                */
/*								    */
/*   PANCAKES <Имя> <Суб-боеприпас> ...                             */
/*         ... <Число суб-боеприпасов> <Шаг выброса> ...            */
/*         ..  <Число кругов> <Радиус круга>                        */

  int  RSS_Module_WarHeadSimple::cPancakes(char *cmd)

{
#define  _COORD_MAX   4
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char  *sub_name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
              RSS_Object  *sub_object ;
                    char   text[1024] ;
                    char  *error ; 
                    char  *end ;
                     int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                 sub_name= pars[1] ;
                      xyz=&pars[2] ;   

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: PANCAKES <Имя_компонентa> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------ Проверка суб-боеприпаса */

    if(sub_name   ==NULL ||
       sub_name[0]==  0    ) {                                      /* Если имя суб-боеприпаса не задано... */
                           SEND_ERROR("Не задано имя суб-боеприпаса. \n"
                                      "Например: PANCAKES <Имя компонентa> <Имя суб-боеприпаса> ...") ;
                                         return(-1) ;
                             }

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, sub_name)) {  sub_object=OBJECTS[i] ;
                                                              break ;         }

         if(i>=OBJECTS_CNT) {
                                sprintf(text, "Объект '%s' не найден ", sub_name) ;
                             SEND_ERROR(text) ;
                                  return(NULL) ;
                            }

#undef   OBJECTS
#undef   OBJECTS_CNT

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
      if(coord_cnt<3)  error="Не указаны параметры БЧ" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

                 unit->load_type  =_PANCAKES_TYPE ;
         strncpy(unit-> sub_unit, sub_name, sizeof(unit->sub_unit)-1) ;
                 unit-> sub_object= sub_object ;
                 unit-> sub_count = (int)coord[0] ;
                 unit-> sub_step  =      coord[1] ;
                 unit-> sub_series= (int)coord[2] ;
                 unit-> sub_range = (int)coord[3] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	   Поиск обьекта типа WarHeadSimple по имени                */

  RSS_Unit *RSS_Module_WarHeadSimple::FindUnit(char *name)

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

     if(strcmp(unit->Type, "WarHeadSimple")) {

           SEND_ERROR("Компонент не является компонентом типа WarHeadSimple") ;
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
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "ПРОСТАЯ БЧ"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_WarHeadSimple::RSS_Unit_WarHeadSimple(void)

{
   strcpy(Type, "WarHeadSimple") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     tripping_type    =_BY_ALTITUDE ;
     tripping_altitude= 0. ;
     tripping_time    = 0. ;

         load_type    =_GRENADE_TYPE ;
          hit_range   = 0. ;
        blast_radius  = 5. ;

          sub_unit[0] = 0 ;
          sub_object  = NULL ;
          sub_count   = 0 ;
          sub_step    = 0. ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_WarHeadSimple::~RSS_Unit_WarHeadSimple(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_WarHeadSimple::vFree(void)

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

    class RSS_Object *RSS_Unit_WarHeadSimple::vCopy(char *name)

{
         RSS_Model_data  create_data ;
 RSS_Unit_WarHeadSimple *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_WarHeadSimple *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

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

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_WarHeadSimple::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_WarHeadSimple::vCalculateStart(double  t)
{
        blast=        0 ;
   start_time=(time_t)t ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_WarHeadSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
       char  text[1024] ;
       char  sub_name[128] ;
     double  k ;
     double  s, dx, dy, dz ;
     double  x_drop, y_drop, z_drop ;
   Matrix2d  Sum_Matrix ;
   Matrix2d  Oper_Matrix ;  
   Matrix2d  Vect_Matrix ;  
     double  step_e, angle_e, x_e, y_e ;
        int  n ;
        int  i ;

/*------------------------------------------------- Входной контроль */

   if(blast)  return(1) ;

/*----------------------------------------------- Приведение времени */

          t1-=this->start_time ;
          t2-=this->start_time ;

/*----------------------------------------- Определение срабатывания */
/*- - - - - - - - - - - - - - - - - - - - - - Срабатывание по высоте */
  if(this->tripping_type==_BY_ALTITUDE)
   if(t2>5.) 
    if(this->Owner->y_base<this->tripping_altitude) {

                                        blast=1 ;

                                     k=(this->tripping_altitude-y)/(this->Owner->y_base-y) ;
                   this->Owner->x_base=x+k*(this->Owner->x_base-x) ;
                   this->Owner->z_base=z+k*(this->Owner->z_base-z) ;
                   this->Owner->y_base=     this->tripping_altitude ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - -  Срабатывание по времени */
  if(this->tripping_type==_BY_TIME)
   if(this->tripping_time> t1 &&
      this->tripping_time<=t2   ) {
                                        blast=1 ;
                                  }
/*------------------------------------------- Отработка срабатывания */

  if(blast) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ОФ */
   if(load_type== _GRENADE_TYPE) {

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC BLAST CR blast_%s %lf %lf %s;"
                                         "START blast_%s;",
                                   this->Owner->Name,
                                   this->Owner->Name, this->hit_range, this->blast_radius, this->Owner->Name,
                                   this->Owner->Name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - -  Последовательный выброс */
   if(load_type==  _STRIPE_TYPE) {

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC SCENE VISIBLE %s 0;",
                                          this->Owner->Name, this->Owner->Name) ;
                           strncat(callback, text, cb_size) ;

          if(this->sub_object!=NULL) {

                       s=sqrt((this->Owner->x_base-x)*(this->Owner->x_base-x)+
                              (this->Owner->y_base-y)*(this->Owner->y_base-y)+
                              (this->Owner->z_base-z)*(this->Owner->z_base-z) ) ;
                      dx=this->sub_step*(this->Owner->x_base-x)/s ;
                      dy=this->sub_step*(this->Owner->y_base-y)/s ;
                      dz=this->sub_step*(this->Owner->z_base-z)/s ;

            for(i=0 ; i<this->sub_count ; i++) {
                           sprintf(sub_name, "%s_sub_%d", this->Owner->Name, i+1) ;

                                    x_drop=this->Owner->x_base+dx*i ;
                                    y_drop=this->Owner->y_base+dy*i ;
                                    z_drop=this->Owner->z_base+dz*i ;

                           sprintf(text, "EXEC %s COPY %s %s %lf %lf %f;"
                                         "START %s;",
                                          this->sub_object->Module->identification, this->sub_unit, sub_name, x_drop, y_drop, z_drop,
                                                   sub_name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                                               }
                                   }
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - Выброс "блинчиками" */
   if(load_type==_PANCAKES_TYPE) {

        Sum_Matrix.Load3d_azim(-this->Owner->a_azim) ;             /* Рассчёт матрицы преобразования */
       Oper_Matrix.Load3d_elev(-this->Owner->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;

                       s=sqrt((this->Owner->x_base-x)*(this->Owner->x_base-x)+
                              (this->Owner->y_base-y)*(this->Owner->y_base-y)+
                              (this->Owner->z_base-z)*(this->Owner->z_base-z) ) ;
                      dx=this->sub_step*(this->Owner->x_base-x)/s ;
                      dy=this->sub_step*(this->Owner->y_base-y)/s ;
                      dz=this->sub_step*(this->Owner->z_base-z)/s ;

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC SCENE VISIBLE %s 0;",
                                          this->Owner->Name, this->Owner->Name) ;
                           strncat(callback, text, cb_size) ;

        if(this->sub_object!=NULL) {

          for(n=0 ; n<this->sub_series ; n++) {

            for(i=0 ; i<this->sub_count/this->sub_series ; i++) {

               step_e=2.*_PI/((double)(this->sub_count/this->sub_series)) ;
              angle_e=ProgramModule.gGaussianValue(i*step_e, step_e/4.) ;
                  x_e=this->sub_range*cos(angle_e) ;
                  y_e=this->sub_range*sin(angle_e) ;

                 Vect_Matrix.LoadZero(3, 1) ;
                 Vect_Matrix.SetCell (0, 0, x_e) ;
                 Vect_Matrix.SetCell (1, 0, y_e) ;
                 Vect_Matrix.SetCell (2, 0, 0) ;
                 Vect_Matrix.LoadMul (&Sum_Matrix, &Vect_Matrix) ;

                   x_drop=this->Owner->x_base+dx*n+Vect_Matrix.GetCell(0, 0) ;
                   y_drop=this->Owner->y_base+dy*n+Vect_Matrix.GetCell(1, 0) ;
                   z_drop=this->Owner->z_base+dz*n+Vect_Matrix.GetCell(2, 0) ;

                           sprintf(sub_name, "%s_sub_%d_%d", this->Owner->Name, n+1, i+1) ;

                           sprintf(text, "EXEC %s COPY %s %s %lf %lf %f;"
                                         "START %s;",
                                          this->sub_object->Module->identification, this->sub_unit, sub_name, x_drop, y_drop, z_drop,
                                                   sub_name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                                                                }
                                              }
                                   }
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       return(1) ;
                 }
/*------------------------------------------------- Фиксация "следа" */

        x=this->Owner->x_base ;
        y=this->Owner->y_base ;
        z=this->Owner->z_base ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_WarHeadSimple::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  Управление БЧ                  	     */

    int  RSS_Unit_WarHeadSimple::vSetWarHeadControl(char *regime)

{
   return(0) ;
}

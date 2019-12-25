/********************************************************************/
/*								    */
/*       МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "КОМБИНИРОВАННАЯ ГСН"        */
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

#include "U_HomingHub.h"

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

     static   RSS_Module_HomingHub  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_HOMING_HUB_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_HOMING_HUB_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                      "КОМБИНИРОВАННАЯ ГСН"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_HomingHub_instr  RSS_Module_HomingHub_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_HomingHub::cHelp   },
 { "config",  "c",  "#CONFIG - задать параметры компонента в диалоговом режиме",
                    " CONFIG <Имя> \n"
                    "   Задать параметры компонента в диалоговом режиме\n",
                    &RSS_Module_HomingHub::cConfig },
 { "plugin",  "p",  "#PLUGIN - задать параметры компонента в диалоговом режиме",
                    " PLUGIN <Имя> <Очередность> <>\n"
                    "   Задать параметры компонента в диалоговом режиме\n",
                    &RSS_Module_HomingHub::cPlugin },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_HomingHub_instr *RSS_Module_HomingHub::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_HomingHub::RSS_Module_HomingHub(void)

{

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="HomingHub" ;
          category="Unit" ;
         lego_type="Homing" ;

        mInstrList=RSS_Module_HomingHub_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_HomingHub::~RSS_Module_HomingHub(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_HomingHub::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_HomingHub *unit ;
                  int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_HomingHub ;
    if(unit==NULL) {
               SEND_ERROR("Секция HomingHub: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Комбинированная ГСН") ;

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

     int  RSS_Module_HomingHub::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Комбинированная ГСН", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_HomingHub::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "HOMINGHUB"
#define  _SECTION_SHRT_NAME   "HOMINGHUB"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object HomingHub:") ;
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
       if(status)  SEND_ERROR("Секция HomingHub: Неизвестная команда") ;
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

  int  RSS_Module_HomingHub::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_HomingHub_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_HomingHub::cInfo(char *cmd)

{
                     char  *name ;
       RSS_Unit_HomingHub  *unit ;
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

       unit=(RSS_Unit_HomingHub *)FindUnit(name) ;                  /* Ищем компонент по имени */
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

  int  RSS_Module_HomingHub::cConfig(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
      RSS_Unit_HomingHub *unit ;
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

       unit=(RSS_Unit_HomingHub *)FindUnit(name) ;                  /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_HomingHub_Config_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PLUGIN                  */
/*								    */
/*    PLUGIN <Имя компонента> <Этап> <Тип компонента>               */

  int  RSS_Module_HomingHub::cPlugin(char *cmd)

{
#define   _PARS_MAX  10

                   char *pars[_PARS_MAX] ;
                   char *name ;
     RSS_Unit_HomingHub *hub ;
                   char *stage ;
                   char *unit_name ;
                   char *unit_type ;
        RSS_Unit_Homing *unit ;
             RSS_Object *object ;
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
                                     SEND_ERROR("Не задан объект управления") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
                    stage=pars[1] ;
                unit_name=pars[2] ;
                unit_type=pars[3] ;

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонентa. \n"
                                      "Например: PLUGIN <Имя_компонентa> ...") ;
                                     return(-1) ;
                         }

       hub=(RSS_Unit_HomingHub *)FindUnit(name) ;                   /* Ищем компонент по имени */
    if(hub==NULL)  return(-1) ;

/*---------------------------------------------- Контроль параметров */

    if(strcmp(stage, "1") &&
       strcmp(stage, "2")   ) {
                           SEND_ERROR("Индекс стадии наведения должен быть 1 или 2") ;
                                     return(-1) ;
                              }
    if(unit_name   ==NULL ||
       unit_name[0]==  0    ) {
                           SEND_ERROR("Не задано имя подключаемого компонента") ;
                                     return(-1) ;
                              }
    if(unit_type   ==NULL ||
       unit_type[0]==  0    ) {
                           SEND_ERROR("Не задан тип подключаемого компонента") ;
                                     return(-1) ;
                              }
/*---------------------------------------------- Контроль заполнения */

    if((stage[0]=='1' && hub->units_1[2]!=NULL) ||
       (stage[0]=='2' && hub->units_2[2]!=NULL)   ) {
             SEND_ERROR("Заполнены все слоты для данной стадии наведения") ;
                                     return(-1) ;
                                                    }
/*-------------------------------- Создание и регистрация компонента */

             object=hub->Owner ;

       unit=AddUnit(object, unit_name, unit_type, error) ;        
    if(unit==NULL) {
                       SEND_ERROR(error) ;
                             return(-1) ;
                   }

    if(stage[0]=='1') {
        for(i=0 ; i<3 ; i++) if(hub->units_1[i]==NULL) {  hub->units_1[i]=unit ;  break ;  }
                      } 
    else              {
        for(i=0 ; i<3 ; i++) if(hub->units_2[i]==NULL) {  hub->units_2[i]=unit ;  break ;  }
                      } 
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*             Поиск обьекта типа HomingHub по имени                */

  RSS_Unit *RSS_Module_HomingHub::FindUnit(char *name)

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

     if(strcmp(unit->Type, "HomingHub")) {

           SEND_ERROR("Компонент не является компонентом типа HomingHub") ;
                            return(NULL) ;
                                         }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*                                                                   */
/*                 Добавление компонента к объекту                   */

  class RSS_Unit_Homing *RSS_Module_HomingHub::AddUnit(RSS_Object *object, char *unit_name, char *unit_type, char *error)

{
        RSS_Kernel *unit_module ;
   RSS_Unit_Homing *unit ;
               int  i ;

/*---------------------------------------- Контроль имени компонента */

   for(i=0 ; i<object->Units.List_cnt ; i++)
     if(!stricmp(unit_name, object->Units.List[i].object->Name))  break ;

      if(i<object->Units.List_cnt) {
            strcpy(error, "В состав объекта уже включен компонент с таким именем") ;
                                     return(NULL) ;
                                   }
/*------------------------------------ Идентификация типа компонента */

#define   MODULES       RSS_Kernel::kernel->modules 
#define   MODULES_CNT   RSS_Kernel::kernel->modules_cnt

                 unit_module=NULL ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit",     MODULES[i].entry->category      ) &&
         !stricmp( unit_type, MODULES[i].entry->identification)   )  unit_module=MODULES[i].entry ;
      
      if(unit_module==NULL) {
                strcpy(error, "Неизвестный тип компонента") ;
                                     return(NULL) ;
                            }

#undef    MODULES
#undef    MODULES_CNT

/*-------------------------------- Создание и регистрация компонента */

        unit=(RSS_Unit_Homing *)unit_module->vCreateObject(NULL) ;
     if(unit==NULL)   return(NULL) ;

        strncpy(unit->Name, unit_name, sizeof(unit->Name)-1) ;
                unit->Owner=object ;

                  object->Units.Add(unit, "") ;

/*-------------------------------------------------------------------*/

  return(unit) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**         ОПИСАНИЕ КЛАССА КОМПОНЕНТА "КОМБИНИРОВАННАЯ ГСН"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_HomingHub::RSS_Unit_HomingHub(void)

{
   strcpy(Type, "HomingHub") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

    memset(units_1, 0, sizeof(units_1)) ;
    memset(units_2, 0, sizeof(units_2)) ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_HomingHub::~RSS_Unit_HomingHub(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_HomingHub::vFree(void)

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

    class RSS_Object *RSS_Unit_HomingHub::vCopy(char *name)

{
         RSS_Model_data  create_data ;
     RSS_Unit_HomingHub *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_HomingHub *)this->Module->vCreateObject(&create_data) ;
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

     int  RSS_Unit_HomingHub::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_HomingHub::vCalculateStart(double  t)
{
   int  i ;


     for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
       if(units_1[i]!=NULL)  units_1[i]->vCalculateStart(t) ;              

     for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
       if(units_2[i]!=NULL)  units_2[i]->vCalculateStart(t) ;              

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_HomingHub::vCalculate(double t1, double t2, char *callback, int cb_size)
{

   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vCalculate(t1, t2, callback, cb_size) ;
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vCalculate(t1, t2, callback, cb_size) ;
                       }

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_HomingHub::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  Управление режимами ГСН         	     */

    int  RSS_Unit_HomingHub::vSetHomingControl(char *regime)

{
   char  text[1024] ;
   char *command ;
   char *next ;
    int  i ;

/*------------------------------------------------------- Подготовка */

            memset(text, 0, sizeof(text)) ;
           strncpy(text, regime, sizeof(text)-1) ;

/*--------------------------------------------------- Перебор команд */

   for(command=text, next=text ; next!=NULL ; command=next+1) {

            next=strchr(command, ';') ;
         if(next!=NULL)  *next=0 ;

/*----------------------------------------------------- Команды хаба */

      if(!memicmp(command, "stage", 5)) {

         if(command[6]=='1')   this->stage=1 ;
         else
         if(command[6]=='2')   this->stage=2 ;
         else                {
                                    sprintf(text, "Illegal stage index in HomingHub : %s", command) ;
                                 SEND_ERROR(text) ;
                                      return(-1) ;          
                             }
                                        }
/*--------------------------------------- Команды модулей управления */

      else                              {

           if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vSetHomingControl(command) ;              
                              }
           else 
           if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vSetHomingControl(command) ;              
                              }

                                        }
/*--------------------------------------------------- Перебор команд */
                                                              }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Направление на цель            	     */

    int  RSS_Unit_HomingHub::vGetHomingDirection(RSS_Point *direction)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingDirection(direction) ;              
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingDirection(direction) ;              
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	 Относительное положение цели           	     */

    int  RSS_Unit_HomingHub::vGetHomingPosition(RSS_Point *position)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingPosition(position) ; 
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingPosition(position) ;
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 Дистанция до цели                 	     */

    int  RSS_Unit_HomingHub::vGetHomingDistance(double *distance)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingDistance(distance) ;              
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingDistance(distance) ;              
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 Скорость сближения с целью            	     */

    int  RSS_Unit_HomingHub::vGetHomingClosingSpeed(double *velocity)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingClosingSpeed(velocity) ;
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingClosingSpeed(velocity) ;              
                       }

   return(0) ;
}



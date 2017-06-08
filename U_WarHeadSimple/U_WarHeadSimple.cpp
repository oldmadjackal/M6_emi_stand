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
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_WarHeadSimple::cInfo },
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
        unit->Features[i]=this->feature_modules[i]->vCreateFeature(unit) ;

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

#define  _SECTION_FULL_NAME   "WARHEAD"
#define  _SECTION_SHRT_NAME   "WH"

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
                    "Range Min   % 5lf\r\n" 
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

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

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
  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_WarHeadSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  static  double  x, y, z ;

   if(t2>5.) 
    if(this->Owner->y_base<=0.) {
                                   strcat(callback, "STOP ") ;
                                   strcat(callback, this->Owner->Name) ;
                                   strcat(callback, ";") ;

                               this->Owner->x_base=x+(this->Owner->x_base-x)*y/(y-this->Owner->y_base) ;
                               this->Owner->z_base=z+(this->Owner->z_base-z)*y/(y-this->Owner->y_base) ;
                               this->Owner->y_base= 0. ;

                                       return(1) ;
                                }

        x=this->Owner->x_base ;
        y=this->Owner->y_base ;
        z=this->Owner->z_base ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_WarHeadSimple::vCalculateShow(void)
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

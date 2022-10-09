/********************************************************************/
/*                                                                  */
/*            МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ТАБЛИЧНАЯ МОДЕЛЬ"      */
/*                                                                  */
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

#include "U_ModelTable.h"

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

     static   RSS_Module_ModelTable  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_MODEL_TABLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_MODEL_TABLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                         "ПРОСТАЯ МОДЕЛЬ"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_ModelTable_instr  RSS_Module_ModelTable_InstrList[]={

 { "help",      "?",  "#HELP - список доступных команд", 
                       NULL,
                      &RSS_Module_ModelTable::cHelp   },
 { "info",      "i",  "#INFO - выдать информацию по объекту",
                      " INFO <Имя> \n"
                      "   Выдать основную информацию по объекту в главное окно\n"
                      " INFO/ <Имя> \n"
                      "   Выдать полную информацию по объекту в отдельное окно",
                      &RSS_Module_ModelTable::cInfo },
 { "pars",      "p",  "#PARS - задание параметров модели в диалоговом режиме", 
                       NULL,
                      &RSS_Module_ModelTable::cPars   },
 { "path",      "pt", "#PATH - задание файла таблицы данных", 
                       NULL,
                      &RSS_Module_ModelTable::cPath   },
 { "format",    "f", "#FORMAT - задание формата данных в таблице", 
                      "  Структура спецификации формата: [DN]P1#P2#...#Pk \n"
                      "  где D - используемый десятичный разделитель: . или , \n"
                      "      N - номер первой строки данных: 1... \n"
                      "      # - разделитель полей данных ';', ',', '#' или '^' \n"
                      "     Pk - обозначения поля данных: \n"
                      "           T - временная метка \n"
                      "           X,Y,Z - координаты \n"
                      "           V - скорость \n"
                      "           Vx, Vy, Vz - проекции скорости \n"
                      "           A, E, R - углы ориентации: азимут/рыскание, угол возвышения/тангаж, крен \n"
                      "           ? - поле игнорируется \n"
                      "   Поля V, Vx, Vy, Vz, A, E, R являются необязательными  \n"
                      "     и при их отсутствии вычисляются по имеющимся \n"
                      " \n"
                      "  Например: [,2]T;V;*;X;Y;Z",
                      &RSS_Module_ModelTable::cFormat  },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_ModelTable_instr *RSS_Module_ModelTable::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_ModelTable::RSS_Module_ModelTable(void)

{

/*---------------------------------------------------- Инициализация */

           keyword="EmiStand" ;
    identification="ModelTable" ;
          category="Unit" ;
         lego_type="Model" ;

        mInstrList=RSS_Module_ModelTable_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_ModelTable::~RSS_Module_ModelTable(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_ModelTable::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_ModelTable *unit ;
                   int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_ModelTable ;
    if(unit==NULL) {
               SEND_ERROR("Секция ModelTable: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Модель табличная") ;

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

     int  RSS_Module_ModelTable::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Табличная модель", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_ModelTable::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MODELTABLE"
#define  _SECTION_SHRT_NAME   "MODELTABLE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object ModelTable:") ;
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
       if(status)  SEND_ERROR("Секция ModelTable: Неизвестная команда") ;
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

  int  RSS_Module_ModelTable::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_ModelTable_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_ModelTable::cInfo(char *cmd)

{
                     char  *name ;
      RSS_Unit_ModelTable  *unit ;
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

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner     %s\r\n" 
                    "Path      %s\r\n" 
                    "Format    %s\r\n" 
                    "\r\n",
                        unit->Name,  unit->Type, 
                        unit->Owner->Name, 
                        unit->path, unit->format 
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

  int  RSS_Module_ModelTable::cPars(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
   RSS_Unit_ModelTable  *unit ;
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
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: PARS <Имя_компонента> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Unit_ModelTable_Pars_dialog, 
                                      (LPARAM)unit                   ) ;
     if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PATH                    */
/*								    */
/*       PATH <Имя> <Путь к файлу>                                  */

  int  RSS_Module_ModelTable::cPath(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *path ;
   RSS_Unit_ModelTable *unit ;
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

                     name=pars[0] ;
                     path=pars[1] ;   

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: PATH <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    if(path==NULL) {
                       SEND_ERROR("Путь файла данных не задан") ;
                                       return(-1) ;
                   }

    if(access(path, 0x00)) {
                       SEND_ERROR("Путь файла данных не доступен или не существует") ;
                                       return(-1) ;
                           }
/*----------------------------------------------------- Пропись пути */

          strncpy(unit->path, path, sizeof(unit->path)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции FORMAT                  */
/*								    */
/*       FORMAT <Имя> <Спецификация формата>                        */

  int  RSS_Module_ModelTable::cFormat(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *format ;
   RSS_Unit_ModelTable *unit  ;
                   int  status ; 
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

                     name=pars[0] ;
                   format=pars[1] ;   

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: FORMAT <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ Разбор параметров */

    if(format==NULL) {
                       SEND_ERROR("Формат данных не задан") ;
                                       return(-1) ;
                     }

       status=unit->iFormatDecode(format) ;
    if(status) {
                  SEND_ERROR("Некорректный формат данных") ;
                               return(-1) ;
               }
/*----------------------------------------------------- Пропись пути */

       strncpy(unit->format, format, sizeof(unit->format)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     Поиск обьекта типа ModelTable по имени                */

  RSS_Unit *RSS_Module_ModelTable::FindUnit(char *name)

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

     if(strcmp(unit->Type, "ModelTable")) {

           SEND_ERROR("Компонент не является компонентом типа ModelTable") ;
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
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "ТАБЛИЧНАЯ МОДЕЛЬ"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_ModelTable::RSS_Unit_ModelTable(void)

{
   strcpy(Type, "ModelTable") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

       memset(path,   0, sizeof(path)) ;
       memset(format, 0, sizeof(format)) ;

              t_0=  0. ; 
             file=NULL ; 
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_ModelTable::~RSS_Unit_ModelTable(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_ModelTable::vFree(void)

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

    class RSS_Object *RSS_Unit_ModelTable::vCopy(char *name)

{
        RSS_Model_data  create_data ;
   RSS_Unit_ModelTable *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_ModelTable *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

/*------------------------------------- Копирование настроек объекта */

             strcpy(unit->path,   this->path) ;
             strcpy(unit->format, this->format) ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_ModelTable::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_ModelTable::vCalculateStart(double  t)
{
  double  pars[20] ;
     int  status ;
    char  text[1024] ;
     int  i ;

/*------------------------------------ Инициализация рабочих данных */

      this->t_0=t ;

   if(this->file!=NULL) {
                            fclose(this->file) ;
                                   this->file=NULL ;
                        }
/*-------------------------------------------------- Парсинг формата */

      status=iFormatDecode(this->format) ;
   if(status) {
                    sprintf(text, "Некорректый формат данных для модуля %s объекта %s",
                                       this->Name, this->owner) ;
                 SEND_ERROR(text) ;
                      return(-1) ;
              }
/*------------------------------------------- Проход заголовка файла */

      file=fopen(this->path, "rt") ;
   if(file==NULL) {
                      sprintf(text, "Ошибка открытия файла данных для модуля %s объекта %s (errno=%d) : %s",
                                       this->Name, this->owner, errno, this->path) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*------------------------------------------- Проход заголовка файла */

   for(i=1 ; i<this->start_row ; i++) {

              fgets(text, sizeof(text)-1, file) ;
                                      }
/*---------------------------------- Определение начальных координат */

             memset(text, 0, sizeof(text)) ;
              fgets(text, sizeof(text)-1, file) ;

      status=iParsDissect(text, pars, sizeof(pars)/sizeof(*pars)) ;
   if(status) {
                    sprintf(text, "Слишком много данных в строке для модуля %s объекта %s",
                                       this->Name, this->owner) ;
                 SEND_ERROR(text) ;
                      return(-1) ;
              }

           memset(        pars_1, 0,            sizeof(pars_1)) ;
           memcpy((void *)pars_2, (void *)pars, sizeof(pars_2)) ;

                                    t_start=pars[this->t_idx] ;

       if(this->azim_idx!=-1)  this->a_azim=pars[this->azim_idx] ;
       if(this->elev_idx!=-1)  this->a_azim=pars[this->elev_idx] ;
       if(this->roll_idx!=-1)  this->a_azim=pars[this->roll_idx] ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_ModelTable::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Object *parent ;
         int  status ;
      double  dt ;
      double  v ;
      double  l ;
      double  lx ;
      double  ly ;
      double  lz ;
        char  text[1024] ;

/*------------------------------------------------ Пoдготовка данных */

           t1=t1-this->t_0+t_start ;
           t2=t2-this->t_0+t_start ;

    if(t1<0.)  return(0) ;

        parent=this->Owner ;

/*----------------------------------------------- Сканирование файла */

  if(pars_2[t_idx]<t2) {                                            /* Если мы выходим из предыдущего "кадра" данных... */

    while(1) {

             memcpy((void *)pars_1, (void *)pars_2, sizeof(pars_1)) ;

             memset(text, 0, sizeof(text)) ;
              fgets(text, sizeof(text)-1, file) ;

         status=iParsDissect(text, pars_2, sizeof(pars_2)/sizeof(*pars_2)) ;
      if(status) {
                       sprintf(text, "Слишком много данных в строке для модуля %s объекта %s",
                                          this->Name, this->owner) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                 }

      if(pars_2[t_idx]>=t2)  break ;

             }
                       } 
/*---------------------------------------------------- Выдача данных */

                dt=(t2-pars_1[t_idx])/(pars_2[t_idx]-pars_1[t_idx]) ;

             parent->x_base=pars_1[x_idx]+(pars_2[x_idx]-pars_1[x_idx])*dt ;
             parent->y_base=pars_1[y_idx]+(pars_2[y_idx]-pars_1[y_idx])*dt ;
             parent->z_base=pars_1[z_idx]+(pars_2[z_idx]-pars_1[z_idx])*dt ;

                         lx=pars_2[x_idx]-pars_1[x_idx] ;
                         ly=pars_2[y_idx]-pars_1[y_idx] ;
                         lz=pars_2[z_idx]-pars_1[z_idx] ;

                         l =sqrt(lx*lx+ly*ly+lz*lz) ;
                      
      if(   v_idx>=0) {
                         v=pars_1[v_idx]+(pars_2[v_idx]-pars_1[v_idx])*dt ;
                      }
      else            { 
                         v= l /(pars_2[t_idx]-pars_1[t_idx]) ;
                      }

      if(  vx_idx>=0) {
                         parent->x_velocity=pars_1[vx_idx]+(pars_2[vx_idx]-pars_1[vx_idx])*dt ;
                      }
      else            { 
                         parent->x_velocity=v*lx/l ;
                      }

      if(  vy_idx>=0) {
                         parent->y_velocity=pars_1[vy_idx]+(pars_2[vy_idx]-pars_1[vy_idx])*dt ;
                      }
      else            { 
                         parent->y_velocity=v*ly/l ;
                      }

      if(  vz_idx>=0) {
                         parent->z_velocity=pars_1[vz_idx]+(pars_2[vz_idx]-pars_1[vz_idx])*dt ;
                      }
      else            { 
                         parent->z_velocity=v*lz/l ;
                      }

      if(azim_idx>=0) {
                         parent->a_azim=pars_1[azim_idx]+(pars_2[azim_idx]-pars_1[azim_idx])*dt ;
                      }
      else            { 
                         parent->a_azim=atan2(parent->x_velocity, parent->z_velocity)*_RAD_TO_GRD ; 
                      }

      if(elev_idx>=0) {
                         parent->a_elev=pars_1[elev_idx]+(pars_2[elev_idx]-pars_1[elev_idx])*dt ;
                      }
      else            { 
                         parent->a_elev=atan2(parent->y_velocity, sqrt(parent->x_velocity*parent->x_velocity+
                                                                       parent->z_velocity*parent->z_velocity ))*_RAD_TO_GRD ; 
                      }

      if(roll_idx>=0) {
                         parent->a_roll=pars_1[roll_idx]+(pars_2[roll_idx]-pars_1[roll_idx])*dt ;
                      }
      else            { 
                         parent->a_roll=0 ; 
                      }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_ModelTable::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}


/*********************************************************************/
/*								     */
/*              Управление аэродинамическими поверхностями     	     */

    int  RSS_Unit_ModelTable::vSetAeroControl(RSS_Unit_Aero_Control *aeros, int  aeros_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Тяга двигателя                	     */

    int  RSS_Unit_ModelTable::vSetEngineThrust(RSS_Unit_Engine_Thrust *thrust, int  thrust_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  Масса и положение центра масс двигателя            */

    int  RSS_Unit_ModelTable::vSetEngineMass(double  mass, RSS_Point *center)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  Моменты инерции двигателя                          */

    int  RSS_Unit_ModelTable::vSetEngineMI(double  Ix, double  Iy, double  Iz)

{
   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Разбор формата записи                          */
/*                                                                   */
/*  Структура спецификации формата: [DN]P1#P2#...#Pk                 */
/*                                                                   */
/* "  где D - используемый десятичный разделитель: '.' или ','       */
/*      N - номер первой строки данных: 1...                         */
/*      # - разделитель полей данных: ';', ',', '#' или '^'          */
/*     Pk - обозначения поля данных:                                 */
/*           T - временная метка                                     */
/*           X,Y,Z - координаты                                      */
/*           V - скорость                                            */
/*           Vx, Vy, Vz - проекции скорости                          */
/*           A, E, R - углы ориентации: азимут угол возвышения, крен */
/*           ? - поле игнорируется                                   */
/*   Поля V, Vx, Vy, Vz, A, E, R являются необязательными            */
/*                                                                   */
/*   Например: [,2]T;V;?;X;Y;Z",                                     */


    int  RSS_Unit_ModelTable::iFormatDecode(char *format)

{
   char  fmt[128] ;
   char *end ;
    int  idx ;
    int  i  ;

/*---------------------------------------------------- Инициализация */

              t_idx=-1 ;
              x_idx=-1 ;
              y_idx=-1 ;
              z_idx=-1 ;
              v_idx=-1 ;
             vx_idx=-1 ;
             vy_idx=-1 ;
             vz_idx=-1 ;
           azim_idx=-1 ;
           elev_idx=-1 ;
           roll_idx=-1 ;

       memset(fmt, 0, sizeof(fmt)) ;
      strncpy(fmt, format, sizeof(fmt)-1) ;

/*-------------------------------------------------- Разбор префикса */

    if(fmt[0]!='[')  return(-1) ;

    if(fmt[1]!='.' &&
       fmt[1]!=','   )  return(-1) ;

       this->dgt_sep=format[1] ;

       this->start_row=strtoul(fmt+2, &end, 10) ;
    if(this->start_row==0)  return(-1) ;

    if(*end!=']')  return(-1) ;

/*------------------------------------------------ Разбор параметров */

#define   IS_SEPARATOR   (fmt[i+1]==';' || fmt[i+1]==',' || fmt[i+1]=='#' || fmt[i+1]=='^')

  for(idx=0, i=end-fmt+1 ; fmt[i] ; i++) {

    if( fmt[i]=='?' && (IS_SEPARATOR || fmt[i+1]==0)) {
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='T' && (IS_SEPARATOR || fmt[i+1]==0)) {
             t_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='X' && (IS_SEPARATOR || fmt[i+1]==0)) {
             x_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='Y' && (IS_SEPARATOR || fmt[i+1]==0)) {
             y_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='Z' && (IS_SEPARATOR || fmt[i+1]==0)) {
             z_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='V' && (IS_SEPARATOR || fmt[i+1]==0)) {
             v_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='A' && (IS_SEPARATOR || fmt[i+1]==0)) {
             azim_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='E' && (IS_SEPARATOR || fmt[i+1]==0)) {
             elev_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='R' && (IS_SEPARATOR || fmt[i+1]==0)) {
             roll_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='V' && fmt[i+1]=='X' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vx_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else
    if( fmt[i]=='V' && fmt[i+1]=='Y' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vy_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else
    if( fmt[i]=='V' && fmt[i+1]=='Z' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vz_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else                                                               {
                                   return(-1) ;
                                                                       }

                                         }

#undef  IS_SEPARATOR

/*--------------------------------- Контроль обязательных параметров */

    if(t_idx==-1 || 
       x_idx==-1 ||
       y_idx==-1 ||
       z_idx==-1   ) {
                        return(-1) ;
                     }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                     Разбор строки данных                          */

    int  RSS_Unit_ModelTable::iParsDissect(char *text, double *pars, int  pars_max)

{
   char *end ;
    int  i  ;

/*---------------------------------------------------- Инициализация */

/*------------------------------------------ Замена десятичной точки */

   if(this->dgt_sep!='.') {

      for(i=0 ; text[i] ; i++)
        if(text[i]==this->dgt_sep)  text[i]='.' ;

                          }
/*---------------------------------------------------- Разбор данных */

   for(i=0 ; i<pars_max ; i++) {

        pars[i]=strtod(text, &end) ;

      if(*end==0)  break ;

         text=end+1 ;

                               }
/*-------------------------------------------------------------------*/

   return(0) ;
}

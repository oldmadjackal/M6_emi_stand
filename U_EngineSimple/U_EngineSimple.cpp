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
#include "..\Ud_tools\UserDlg.h"

#include "U_EngineSimple.h"

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

     static   RSS_Module_EngineSimple  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_ENGINE_SIMPLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_ENGINE_SIMPLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                  "ПРОСТОЙ РАКЕТНЫЙ ДВИГАТЕЛЬ"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/


/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_EngineSimple_instr  RSS_Module_EngineSimple_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_EngineSimple::cHelp   },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_EngineSimple::cInfo },
 { "profile", "p",  "#PROFILE - загрузка профиля работы",
                    " PROFILE <Имя> <Имя файла профиля>\n"
                    "   Загрузить профиль работы из файла",
                    &RSS_Module_EngineSimple::cProfile },
 { "sigma",   "s",  "#SIGMA - средне-квадратичное отклонение параметров",
                    " SIGMA <Имя> <Значение>\n"
                    "   Задать относительное средне-квадратичное отклонение параметров двигателя от эталонного",
                    &RSS_Module_EngineSimple::cSigma },
 { "view",    "v",  "#VIEW - просмотр профиля работы",
                    " VIEW <Имя>\n"
                    "   Показать графики профиля работы",
                    &RSS_Module_EngineSimple::cView },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_EngineSimple_instr *RSS_Module_EngineSimple::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_EngineSimple::RSS_Module_EngineSimple(void)

{
  static  WNDCLASS  View_wnd ;
              char  text[1024] ;

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="EngineSimple" ;
          category="Unit" ;
         lego_type="Engine" ;

        mInstrList=RSS_Module_EngineSimple_InstrList ;

/*--------------------------- Регистрация класса окна UD_Show_view2D */

	View_wnd.lpszClassName="U_EngineSimple_view_class" ;
	View_wnd.hInstance    = GetModuleHandle(NULL) ;
	View_wnd.lpfnWndProc  = UD_diagram_2D_prc ;
	View_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	View_wnd.hIcon        =  NULL ;
	View_wnd.lpszMenuName =  NULL ;
	View_wnd.hbrBackground=  NULL ;
	View_wnd.style        =    0 ;
	View_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&View_wnd)) {
              sprintf(text, "U_EngineSimple_view_class register error %d", GetLastError()) ;
           SEND_ERROR(text) ;
                    return ;
                                  }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_EngineSimple::~RSS_Module_EngineSimple(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_EngineSimple::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_EngineSimple *unit ;
                     int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_EngineSimple ;
    if(unit==NULL) {
               SEND_ERROR("Секция EngineSimple: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Ракетный двигатель (простой)") ;

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

     int  RSS_Module_EngineSimple::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Простой ракетный двигатель", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_EngineSimple::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "ENGINESIMPLE"
#define  _SECTION_SHRT_NAME   "ENGINE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object EngineSimple:") ;
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
       if(status)  SEND_ERROR("Секция EngineSimple: Неизвестная команда") ;
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

  int  RSS_Module_EngineSimple::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_EngineSimple_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_EngineSimple::cInfo(char *cmd)

{
                     char  *name ;
    RSS_Unit_EngineSimple  *unit ;
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

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* Ищем компонент по имени */
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
/*		      Реализация инструкции SIGMA                   */
/*								    */
/*      SIGMA <Имя> <Значение>                                      */

  int  RSS_Module_EngineSimple::cSigma(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: SIGMA <Имя_компонента> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------- Пропись значения */

  if(pars[1]==NULL) {
                       SEND_ERROR("Не задано значение относительного средне-квадратичного отклонения.\n"
                                  "Например: SIGMA <Имя> <Значение>") ;
                                     return(-1) ;
                    }

        unit->sigma2=strtod(pars[1], &end) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PROFILE                 */
/*								    */
/*      PROFILE <Имя> <Файл программы>                              */

  int  RSS_Module_EngineSimple::cProfile(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  status ;
                     int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: PROFILE <Имя_компонента> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Считывание данных из файла */

  if(pars[1]==NULL) {
                       SEND_ERROR("Не задано имя файла профиля работы.\n"
                                  "Например: PROFILE <Имя> <файл программы>") ;
                                     return(-1) ;
                    }

        status=ReadProfile(unit, pars[1]) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции VIEW                    */
/*								    */
/*      VIEW <Имя>                                                  */

  int  RSS_Module_EngineSimple::cView(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: VIEW <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------------- Отображение данных */

    DialogBoxIndirectParam(GetModuleHandle(NULL),
		           (LPCDLGTEMPLATE)Resource("IDD_VIEW", RT_DIALOG),
			     GetActiveWindow(), Unit_EngineSimple_View_dialog, (LPARAM)unit) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*	   Поиск обьекта типа EngineSimple по имени                 */

  RSS_Unit *RSS_Module_EngineSimple::FindUnit(char *name)

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

     if(strcmp(unit->Type, "EngineSimple")) {

           SEND_ERROR("Компонент не является компонентом типа EngineSimple") ;
                            return(NULL) ;
                                            }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*								     */
/*              Считывание файла описания профиля работы             */
/*								     */
/*   Структура файла:						     */
/*  FIELDS <Имя 1> ... <Имя N>  				     */
/*  <Время 1> <Значение 1> ... <Значение N>			     */
/*  . . . 							     */
/*  <Время K> <Значение 1> ... <Значение N>			     */
/*								     */
/*  FIELDS Т M                                                       */
/*  0.00 100 21                                                      */
/*  1.89 100  0                                                      */
/*  1.90   0  0                                                      */


  int  RSS_Module_EngineSimple::ReadProfile(RSS_Unit_EngineSimple *unit, char *path)

{
                           FILE *file ;
                           char *fields[30] ;
                           char  fields_text[1024] ;
  RSS_Unit_EngineSimple_Profile  frame ;
                           char  text[1024] ;
                           char  error[1024] ;
                           char *words[30] ;
                           char *end ;
                            int  row ;
                            int  i ;

/*--------------------------------------------------- Открытие файла */

       file=fopen(path, "rt") ;
    if(file==NULL) {
                          sprintf(error, "Ошибка %d открытия файла %s", errno, path) ;
                       SEND_ERROR(error) ;
                                       return(-1) ;
                   }
/*------------------------------------------------ Считывание данных */

       memset(fields,      0, sizeof(fields     )) ;
       memset(fields_text, 0, sizeof(fields_text)) ;

        unit->profile_cnt=0 ;

                      row=0 ;

   while(1) {                                                       /* CIRCLE.1 - Построчно читаем файл */

/*-------------------------------------- Считывание очередной строки */

                      row++ ;

                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* Считываем строку */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* Проходим комментарий */

               end=strchr(text, '\n') ;                             /* Удаляем символ конца строки */
            if(end!=NULL)  *end=0 ;
               end=strchr(text, '\r') ;
            if(end!=NULL)  *end=0 ;

/*----------------------------------- Обработка спецификатора FIELDS */

#define  _KEY_WORD  "FIELDS "
      if(!memicmp(text, _KEY_WORD, strlen(_KEY_WORD))) {

         if(fields[0]!=NULL) {
                                sprintf(error, "Строка %d - профиль должен содержать единственный оператор FIELDS", row) ;
                             SEND_ERROR(error) ;
                                  return(-1) ;
                             }
        

               end=text+strlen(_KEY_WORD) ;
            if(*end==0) {
                          sprintf(error, "Строка %d - оператор FIELDS не содержит перечня полей", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }  

                strncpy(fields_text, end, sizeof(fields_text)) ;

                      i = 0 ;
               fields[i]=strtok(fields_text, " \t") ;

         while(fields[i]!=NULL && i<30) {
                      i++ ;
               fields[i]=strtok(NULL, " \t") ;
                                        }

                                       continue ;
                                                       }
#undef   _KEY_WORD

/*------------------------------------------- Разбор строки на слова */

         if(fields[0]==NULL) {
                                sprintf(error, "Строка %d - профиль должен начинаться с оператора FIELDS", row) ;
                             SEND_ERROR(error) ;
                                  return(-1) ;
                             }
        
           memset(words, 0, sizeof(words)) ;

                   i = 0 ;
             words[i]=strtok(text, " \t") ;

       while(words[i]!=NULL && i<30) {
                   i++ ;
             words[i]=strtok(NULL, " \t") ;
                                     }
/*---------------------------------------------- Формирование записи */

          memset(&frame, 0, sizeof(frame)) ;

                  frame.t=strtod(words[0], &end) ;

       for(i=0 ; i<30 ; i++) {

              if(words[i+1]==NULL)  break ;

              if(fields[i]==NULL) {
                                        sprintf(error, "Строка %d содержит больше параметров, чем указано в строке FIELDS", row) ;
                                     SEND_ERROR(error) ;
                                           return(-1) ;
                                  } 

              if(!stricmp(fields[i], "T" ))  frame.thrust=strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "M" ))  frame.mass  =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CX"))  frame.Cx    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CY"))  frame.Cy    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CZ"))  frame.Cz    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MX"))  frame.Mx    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MY"))  frame.My    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MZ"))  frame.Mz    =strtod(words[i+1], &end) ;
         else                              {
                        sprintf(error, "Неизвестный параметр '%s', возможны T, M, CX, CY, CZ, MX, MY, MZ", fields[i]) ;
                     SEND_ERROR(error) ;
                           return(-1) ;
                                           }
                             }

              if(fields[i]!=NULL) {
                                       sprintf(error, "Строка %d содержит меньше параметров, чем указано в строке FIELDS", row) ;
                                    SEND_ERROR(error) ;
                                          return(-1) ;
                                  } 

/*--------------------------------------- Сохранение кадра программы */

          unit->profile=(RSS_Unit_EngineSimple_Profile *)
                          realloc(unit->profile, (unit->profile_cnt+1)*sizeof(*unit->profile)) ;

          unit->profile[unit->profile_cnt]=frame ;
                        unit->profile_cnt++ ;

/*-------------------------------------------------------------------*/
            }                                                       /* CONTINUE.1 */
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*-------------------------------------- Разметка используемых полей */

                                             unit->use_mass=0 ;
                                             unit->use_Cxyz=0 ;
                                             unit->use_Mxyz=0 ;

       for(i=0 ; i<30 ; i++) {

              if(fields[i]==NULL)  break ;

              if(!stricmp(fields[i], "M" ))  unit->use_mass=1 ;
         else if(!stricmp(fields[i], "CX"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "CY"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "CZ"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "MX"))  unit->use_Mxyz=1 ;
         else if(!stricmp(fields[i], "MY"))  unit->use_Mxyz=1 ;
         else if(!stricmp(fields[i], "MZ"))  unit->use_Mxyz=1 ;
                             }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "ПРОСТОЙ РД"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_EngineSimple::RSS_Unit_EngineSimple(void)

{
   strcpy(Type, "EngineSimple") ;
          Module=&ProgramModule ;

      profile    =NULL ;
      profile_cnt=  0 ;

       sigma2    =  0. ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_EngineSimple::~RSS_Unit_EngineSimple(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_EngineSimple::vFree(void)

{
  int  i ;


  if(this->profile!=NULL)  free(this->profile) ;

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

    class RSS_Unit *RSS_Unit_EngineSimple::vCopy(RSS_Object *owner)

{
        RSS_Model_data  create_data ;
 RSS_Unit_EngineSimple *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_EngineSimple *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

      strcpy(unit->Name, this->Name) ; 
             unit->Owner=owner ;

    if(owner!=NULL)  owner->Units.Add(unit, "") ;

/*------------------------------------- Копирование настроек объекта */

             unit->profile=(RSS_Unit_EngineSimple_Profile *)
                             calloc(this->profile_cnt, sizeof(*unit->profile)) ;
      memcpy(unit->profile, this->profile, 
                            this->profile_cnt*sizeof(*unit->profile)) ;
             unit->profile_cnt=this->profile_cnt ;

             unit->use_mass=this->use_mass ;
             unit->use_Cxyz=this->use_Cxyz ;
             unit->use_Mxyz=this->use_Mxyz ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_EngineSimple::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_EngineSimple::vCalculateStart(double t)
{
       this->t_0       =t ;
       this->real_value=1. ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_EngineSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  double  t ;
  double  k ;
     int  i ;

/*------------------------------------------------ Приведение данных */

           t1-=this->t_0 ;
           t2-=this->t_0 ;

/*-------------------------------------------- Определение диапазона */

               t=0.5*(t2+t1) ;

   for(i=0 ; i<this->profile_cnt ; i++)
     if(t<=this->profile[i].t)  break ;

                                values.t     =t ;

          if(i==  0        ) {
                                values.thrust=0. ;
                                values.mass  =0. ;
                                values.Cx    =0. ;
                                values.Cy    =0. ;
                                values.Cz    =0. ;
                                values.Mx    =0. ;
                                values.My    =0. ;
                                values.Mz    =0. ;
                             }
     else if(i==profile_cnt) {
                                values.thrust=profile[i-1].thrust ;
                                values.mass  =profile[i-1].mass ;
                                values.Cx    =profile[i-1].Cx ;
                                values.Cy    =profile[i-1].Cy ;
                                values.Cz    =profile[i-1].Cz ;
                                values.Mx    =profile[i-1].Mx ;
                                values.My    =profile[i-1].My ;
                                values.Mz    =profile[i-1].Mz ;
                             }
     else                    {
                                       k     =(t-profile[i-1].t)/(profile[i].t-profile[i-1].t) ;
                                values.thrust=profile[i-1].thrust+k*(profile[i].thrust-profile[i-1].thrust) ;
                                values.mass  =profile[i-1].mass  +k*(profile[i].mass  -profile[i-1].mass  ) ;
                                values.Cx    =profile[i-1].Cx    +k*(profile[i].Cx    -profile[i-1].Cx    ) ;
                                values.Cy    =profile[i-1].Cy    +k*(profile[i].Cy    -profile[i-1].Cy    ) ;
                                values.Cz    =profile[i-1].Cz    +k*(profile[i].Cz    -profile[i-1].Cz    ) ;
                                values.Mx    =profile[i-1].Mx    +k*(profile[i].Mx    -profile[i-1].Mx    ) ;
                                values.My    =profile[i-1].My    +k*(profile[i].My    -profile[i-1].My    ) ;
                                values.Mz    =profile[i-1].Mz    +k*(profile[i].Mz    -profile[i-1].Mz    ) ;
                             }

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_EngineSimple::vCalculateShow(double  t1, double t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  Управление двигателем         	     */

    int  RSS_Unit_EngineSimple::vSetEngineControl(RSS_Unit_Engine_Control *engines, int  engines_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Тяга двигателя                	     */

    int  RSS_Unit_EngineSimple::vGetEngineThrust(RSS_Unit_Engine_Thrust *thrust)

{
   thrust->x= 0. ;
   thrust->y= 0. ;
   thrust->z=values.thrust ;

   return(1) ;
}


/*********************************************************************/
/*								     */
/*             	  Масса и положение центра масс двигателя            */

    int  RSS_Unit_EngineSimple::vGetEngineMass(double *mass, RSS_Point *center)

{
                               *mass=values.mass ;

  if(this->use_Cxyz) {
                        center->mark= 1 ;
                        center->x   =values.Cx ;
                        center->y   =values.Cy ;
                        center->z   =values.Cz ;
                     }
  else               {
                        center->mark= 0 ;
                        center->x   = 0. ;
                        center->y   = 0. ;
                        center->z   = 0. ;
                     }

   return(this->use_mass) ;
}


/*********************************************************************/
/*								     */
/*             	  Моменты инерции двигателя                          */

    int  RSS_Unit_EngineSimple::vGetEngineMI(double *Ix, double *Iy, double *Iz)

{
     *Ix=values.Mx ;
     *Iy=values.My ;
     *Iz=values.Mz ;

   return(this->use_Mxyz) ;

}



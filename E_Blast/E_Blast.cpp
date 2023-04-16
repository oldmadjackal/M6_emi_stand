/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ЭФФЕКТОМ "ВЗРЫВ"                  */
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
#include "..\F_Hit\F_Hit.h"

#include "E_Blast.h"

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

     static   RSS_Module_Blast  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 E_BLAST_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 E_BLAST_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**    ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ЭФФЕКТОМ "ВЗРЫВ"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Blast_instr  RSS_Module_Blast_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Blast::cHelp   },
 { "create",  "cr", "#CREATE - создать объект-эффект",
                    " CREATE[/G] <Имя> <Радиус поражения> <Радиус вспышки> <Имя родителя>\n"
                    "   Создает обьект-эффект 'Наземный взрыв' в базовой точке объекта-родителя"
                    " CREATE[/G] <Имя> <Радиус поражения> <Радиус вспышки> <X> <Y> <Z>\n"
                    "   Создает обьект-эффект 'Наземный взрыв' в заданной точке\n"
                    " CREATE/A <Имя> ...\n"
                    "   Создает обьект-эффект 'Воздушный взрыв' аналогично 'Наземному взрыву'",
                    &RSS_Module_Blast::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_Blast::cInfo },
 { "range",   "r",  "#RANGE - задать радиус взрыва",
                    " RANGE <Имя> <Радиус>\n"
                    "   Задать радиус взрыва",
                    &RSS_Module_Blast::cRange },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Blast_instr *RSS_Module_Blast::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Blast::RSS_Module_Blast(void)

{
  static  WNDCLASS  Indicator_wnd ;

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="Blast" ;
          category="Effect" ;

        mInstrList=RSS_Module_Blast_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Blast::~RSS_Module_Blast(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Blast::vCreateObject(RSS_Model_data *data)

{
        std::string  decl ;
   RSS_Effect_Blast *effect ;
    RSS_Feature_Hit *hit ; 
               char *end ; 
                int  i ;
 
#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------- Создание объекта */

       effect=new RSS_Effect_Blast ;
    if(effect==NULL) {
               SEND_ERROR("Секция BLAST: Недостаточно памяти для создания эффекта") ;
                        return(NULL) ;
                     }

             effect->Module=this ;

      strcpy(effect->Decl, "Взрыв") ;

/*---------------------------------- Создание списка свойств объекта */

        effect->Features_cnt=this->feature_modules_cnt ;
        effect->Features    =(RSS_Feature **)
                               calloc(this->feature_modules_cnt, 
                                       sizeof(effect->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++) {
        effect->Features[i]=this->feature_modules[i]->vCreateFeature(effect, NULL) ;
                                                }
/*---------------------------------------- Формирование Hit-свойства */

    for(i=0 ; i<effect->Features_cnt ; i++)
      if(!strcmp(effect->Features[i]->Type, "Hit"))
              hit=(RSS_Feature_Hit *)effect->Features[i] ;

                             decl="$BODY BLAST\r\n"
                                  "$VERTEX001=0. ; 0. ; 0.\r\n"
                                  "$FACET=1\r\n" ;

         hit->vBodyPars(NULL, NULL) ;
         hit->vReadSave("FEATURE HIT", &decl, "Blast.1") ;

      if(*data->pars[0].value!=0)  hit->hit_range =strtod(data->pars[0].value, &end) ;

      if(hit->hit_range>0.)  hit->any_weapon= 1 ;
      else                   hit->any_weapon= 0 ;

/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция BLAST: Переполнение памяти") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=effect ;
       strcpy(OBJECTS[OBJECTS_CNT]->Name, data->name) ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(effect) ;
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Blast::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Взрыв", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_Blast::vShow(char *layer)

{
     int  status ;
     int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
  
/*--------------------------------------------- Отображение эффектов */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - Перебор контекстов отображения */

     for(i=0 ; i<OBJECTS_CNT ; i++) {                               /* LOOP - Перебор маркеров */

       if(stricmp(OBJECTS[i]->Type, "Blast"))  continue ;

           ((RSS_Effect_Blast *)OBJECTS[i])->iShowBlast() ;
                                    }                               /* ENDLOOP - Перебор маркеров */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - Перебор контекстов отображения */
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Blast::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "BLAST"
#define  _SECTION_SHRT_NAME   "BLAST"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Effect Blast:") ;
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
       if(status)  SEND_ERROR("Секция Blast: Неизвестная команда") ;
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

    void  RSS_Module_Blast::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
//  RSS_Unit_EWunit *unit ;
               char *entry ;
               char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE BLAST\n", 
                      strlen("#BEGIN MODULE BLAST\n")) &&
      memicmp(data->c_str(), "#BEGIN EFFECT BLAST\n", 
                      strlen("#BEGIN EFFECT BLAST\n"  ))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*---------------------------------------------- Создание компонента */

   if(!memicmp(buff, "#BEGIN EFFECT BLAST\n", 
              strlen("#BEGIN EFFECT BLAST\n"))) {                   /* IF.1 */
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
                                                }                   /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Blast::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE BLAST\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Blast::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Effect_Blast_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*    CREATE[/G] <Имя> <Радиус поражения> <Радиус взрыва> ...       */
/*             ... <Объект-родитель>...                             */
/*    CREATE[/G] <Имя> <Радиус поражения> <Радиус взрыва> ...       */
/*             ... <X> <Y> <Z>                                      */
/*    CREATE/A <Имя> <Радиус поражения> <Радиус взрыва> ...         */
/*             ... <Объект-родитель>...                             */
/*    CREATE/A <Имя> <Радиус поражения> <Радиус взрыва> ...         */
/*             ... <X> <Y> <Z>                                      */

  int  RSS_Module_Blast::cCreate(char *cmd)

{
   RSS_Model_data  data ;
 RSS_Effect_Blast *object ;
       RSS_Object *parent ;
             char *name ;
              int  a_flag ; 
              int  g_flag ; 
             char *pars[10] ;
           double  x, y, z ;
             char *end ;
              int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          a_flag=0 ;
                          g_flag=0 ;

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
                if(strchr(cmd, 'g')!=NULL ||
                   strchr(cmd, 'G')!=NULL   )  g_flag=1 ;

                           cmd=end+1 ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
     for(i=0 ; i<10 ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя объекта */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<10 ; i++) {                                        /* Извлекаем параметры */
               pars[i]=end+1 ;            
                   end=strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                           }
      } while(0) ;                                                  /* BLOCK.1 */

/*------------------------------------------ Анализ объекта-родителя */

                                x=0. ;
                                y=0. ;
                                z=0. ;

    if( pars[4]!=NULL &&
       *pars[4]!=  0    ) {
                                x=strtod(pars[2], &end) ; ;
                                y=strtod(pars[3], &end) ; ;
                                z=strtod(pars[4], &end) ; ;
                          }
    else                  {
                                parent=FindObject(pars[2], 0) ;
                             if(parent==NULL)  return(-1) ;

                                x=parent->x_base ;
                                y=parent->y_base ;
                                z=parent->z_base ;
                          }
/*--------------------------------- Подготовка блока данных создания */

         memset(&data, 0, sizeof(data)) ;

        strncpy(data.name,  name,  sizeof(data.name)) ;

   if( pars[0]!=NULL ||
      *pars[0]!=  0    ) {
        strncpy(data.pars[0].text, "R",      sizeof(data.pars[0].text )-1) ;
        strncpy(data.pars[0].value, pars[0], sizeof(data.pars[0].value)-1) ;
                         }

        object=(RSS_Effect_Blast *)vCreateObject(&data) ;
     if(object==NULL)  return(0) ;

/*---------------------------------------- Пропись параметров взрыва */

   if(a_flag)  object->blast_type=   _AIR_BLAST ;
   else        object->blast_type=_GROUND_BLAST ;

   if( pars[0]!=NULL ||
      *pars[0]!=  0    )  object->hit_range=strtod(pars[0], &end) ;

   if( pars[1]!=NULL ||
      *pars[1]!=  0    )  object->range_max=strtod(pars[1], &end) ;
   else                   object->range_max=  object->hit_range ;

                        object->x_base=x ;
                        object->y_base=y ;
                        object->z_base=z ;

       for(i=0 ; i<object->Features_cnt ; i++)                      /* Перенос на Свойства */
         object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                                   object->y_base, 
                                                   object->z_base ) ;

/*-------------------------------------------------- Отрисовка сцены */

                      this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Blast::cInfo(char *cmd)

{
               char  *name ;
   RSS_Effect_Blast  *effect ;
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

/*------------------------------------------- Контроль имени эффекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя эффекта. \n"
                                      "Например: INFO <Имя_эффекта> ...") ;
                                     return(-1) ;
                         }

       effect=(RSS_Effect_Blast *)FindObject(name, 1) ;             /* Ищем эффект по имени */
    if(effect==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 7.3lf\r\n" 
                    "     Y % 7.3lf\r\n" 
                    "     Z % 7.3lf\r\n"
                    "Range  % 5lf\r\n" 
                    "\r\n",
                        effect->Name, effect->Type, 
                        effect->x_base, effect->y_base, effect->z_base,
                        effect->range_max
                    ) ;

           info=text ;

/*----------------------------------------------- Запрос на Свойства */

   for(i=0 ; i<effect->Features_cnt ; i++) {

                      effect->Features[i]->vGetInfo(&f_info) ;
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
/*		      Реализация инструкции RANGE                   */
/*								    */
/*       RANGE <Имя> <Радиус взрыва>                                */

  int  RSS_Module_Blast::cRange(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
   RSS_Effect_Blast  *effect ;
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
                      SEND_ERROR("Не задано имя эффекта. \n"
                                 "Например: BLAST <Имя_эфекта> ...") ;
                                     return(-1) ;
                   }

       effect=(RSS_Effect_Blast *)FindObject(name, 1) ;             /* Ищем компонента по имени */
    if(effect==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение радиуса") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указан радиус взрыва" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*--------------------------------------------------- Пропись радиус */

                 effect->range_max=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа BLAST по имени                */

  RSS_Object *RSS_Module_Blast::FindObject(char *name, int  check_type)

{
     char   text[1024] ;
      int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- Поиск объекта по имени */ 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------------- Контроль типа объекта */ 

    if(check_type)
     if(strcmp(OBJECTS[i]->Type, "Blast")) {

           SEND_ERROR("Объект не является объектом типа BLAST") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**							           **/
/**               ОПИСАНИЕ КЛАССА ЭФФЕКТА "ВЗРЫВ"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Effect_Blast::RSS_Effect_Blast(void)

{
   strcpy(Type, "Blast") ;

  Context        =new RSS_Transit_Blast ;
  Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

       color      =RGB(255,   0,   0) ;
       blast_type =_GROUND_BLAST ;
         hit_range= 3. ;
       range_max  = 3. ;
       range      = 0. ;

        x_base    =0. ;
        y_base    =0. ;
        z_base    =0. ;

        x_velocity=0. ;
        y_velocity=0. ;
        z_velocity=0. ;

        dlist1_idx=0 ;
        dlist2_idx=0 ;

}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Effect_Blast::~RSS_Effect_Blast(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Effect_Blast::vFree(void)

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

      if(this->dlist1_idx)
           RSS_Kernel::display.ReleaseList(this->dlist1_idx) ;

      if(this->dlist2_idx)
           RSS_Kernel::display.ReleaseList(this->dlist2_idx) ;

}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Effect_Blast::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN EFFECT BLAST\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n", this->Name) ;  *text+=field ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Effect_Blast::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Effect_Blast::vCalculateStart(double  t)
{
   this->t_0=t ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Effect_Blast::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  char  cb_command[1024] ;

#define  _T1_MAX  0.5
#define  _T2_MAX  1.0

/*------------------------------------------------------- Подготовка */

         t2-=this->t_0 ;

/*----------------------------------------- Анализ завершения взрыва */

      if(t2>_T2_MAX) {
                       sprintf(cb_command, "kill %s;", this->Name) ;
                       strncat(callback, cb_command, cb_size-1) ;

                         range=0. ;
               
                         return(1) ;
                     }
/*-------------------------------------------- Расчет радиуса взрыва */

      if(t2<_T1_MAX) {
                           range=range_max*t2/_T1_MAX ;
                     }
      else           {
                           range=range_max ;
                     } 
/*-------------------------------------------------------------------*/
 
#undef  _T_MAX

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Effect_Blast::vCalculateShow(double  t1, double  t2)
{
   int i ;


   for(i=0 ; i<this->Features_cnt ; i++) {                          /* Отображение объекта */
     this->Features[i]->vBodyBasePoint(NULL, this->x_base, 
                                             this->y_base, 
                                             this->z_base ) ;
     this->Features[i]->vBodyAngles   (NULL, this->a_azim, 
                                             this->a_elev, 
                                             this->a_roll ) ;
     this->Features[i]->vShow         (NULL) ;
                                         }

  return(0) ;
}


/********************************************************************/
/*								    */
/*                        Обработка событий                         */

     int  RSS_Effect_Blast::vEvent(char *event_name, double  t, char *callback, int cb_size)
{
    RSS_Feature_Hit *hit ; 
                int  i ;

/*--------------------------------------------------- Поражение цели */

   if(!stricmp(event_name, "HIT")) {

    for(i=0 ; i<this->Features_cnt ; i++)
      if(!strcmp(this->Features[i]->Type, "Hit"))
              hit=(RSS_Feature_Hit *)this->Features[i] ;

              hit->any_weapon=0 ;

                     return(1) ;
                                   }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           Отображение эффекта с передачей контекста              */

  int  RSS_Effect_Blast::iShowBlast_(void)

{
       strcpy(Context->action, "SHOW") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Отображение эффекта                           */

  int  RSS_Effect_Blast::iShowBlast(void)

{
     int  status ;


     if(dlist2_idx==0)  dlist2_idx=RSS_Kernel::display.GetList(1) ;
     if(dlist1_idx==0)  dlist1_idx=RSS_Kernel::display.GetList(0) ;

     if(dlist1_idx==0 || 
        dlist2_idx==0   )  return(-1) ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------------- Формирование метки взрыва */

             glNewList(dlist2_idx, GL_COMPILE) ;                    /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
              glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
//         glDepthMask(0) ;                                         /* Откл.запись Z-буфера */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Воздушный взрыв */
        if(blast_type==_AIR_BLAST) {

             iShowSphere(0.6, 8, RGB(255, 170,   0)) ;
             iShowSphere(0.8, 8, RGB(255,  85,   0)) ;
             iShowSphere(1.,  8, RGB(255,   0,   0)) ;

                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Наземный взрыв */
        else                       {                                     
 
               iShowCone(1.,  0.5, 12, RGB(255,   0,   0)) ;
               iShowCone(0.8, 0.8, 14, RGB(255,  85,   0)) ;
               iShowCone(0.6, 1.1, 16, RGB(255, 170,   0)) ;
               iShowCone(0.4, 1.4, 12, RGB(255, 255,   0)) ;

                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
//         glDepthMask(1) ;                                         /* Вкл.запись Z-буфера */
//           glDisable(GL_BLEND) ;                                  /* Откл.смешивание цветов */
             glEndList() ;                                          /* Закрытие группы */

/*------------------ Формирование позиционирующей последовательности */

             glNewList(dlist1_idx, GL_COMPILE) ;                    /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(x_base, y_base-0.2, z_base) ;

              glScaled(range, range, range) ;

            glCallList(dlist2_idx) ;                                /* Отрисовка маркера */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Отрисовка воронки                             */

  void  RSS_Effect_Blast::iShowCone(double  r, double  h, int  segments, COLORREF  color)

{
  double  a_step ;
  double  a  ;
     int  i ;


              a_step=2.0*_PI/(double)segments ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 1.) ;

      for(i=0 ; i<segments ; i++) {

                      a=a_step*i ;

                       glBegin(GL_POLYGON) ;
                    glVertex3d(      0.,            0.,       0.          ) ;
                    glVertex3d(r*sin(a          ),  h,  r*cos(a          )) ;
                    glVertex3d(r*sin(a+a_step/2.),  h,  r*cos(a+a_step/2.)) ;
	                 glEnd();

                                  }
}


/********************************************************************/
/*								    */
/*		      Отрисовка сферы                               */

  void  RSS_Effect_Blast::iShowSphere(double  r, int  segments, COLORREF  color)

{
  double  a_step ;
  double  a  ;
  double  x  ;
  double  y  ;
     int  step ;
     int  i ;


              a_step=2.0*_PI/(double)segments ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 1.) ;

      for(step=0 ; step<3 ; step++) {

                       glBegin(GL_POLYGON) ;

      for(i=0 ; i<segments ; i++) {

                      a=a_step*i ;
                      x=r*sin(a) ;
                      y=r*cos(a) ;

          if(step==0)  glVertex3d(x,  y,  0.) ;
          if(step==1)  glVertex3d(0., x,  y ) ;
          if(step==2)  glVertex3d(y,  0., x ) ;

                                  }

	                 glEnd();

                                    }
}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit_Blast::RSS_Transit_Blast(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Blast::~RSS_Transit_Blast(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Blast::vExecute(void)

{

   if(!stricmp(action, "SHOW" ))  ((RSS_Effect_Blast *)object)->iShowBlast() ;

   return(0) ;
}

/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "РАКЕТА"  		    */
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
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\F_Show\F_Show.h"
#include "..\F_Hit\F_Hit.h"

#include "O_Missile.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

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

     static   RSS_Module_Missile  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_MISSILE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_MISSILE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЛЕТУН"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Missile_instr  RSS_Module_Missile_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Missile::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_Missile::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_Missile::cInfo },
 { "owner",   "o",  "#OWNER - назначить носитель ракеты",
                    " OWNER <Имя> <Носитель>\n"
                    "   Назначить объект - носитель ракеты",
                    &RSS_Module_Missile::cOwner },
 { "target",  "tg", "#TARGET - назначить цель ракеты",
                    " TARGET <Имя> <Цель>\n"
                    "   Назначить объект - цель ракеты",
                    &RSS_Module_Missile::cTarget },
 { "base",    "b", "#BASE - задать базовую точку объекта",
                    " BASE <Имя> <x> <y> <z>\n"
                    "   Задает базовую точку объекта\n"
                    " BASE/x <Имя> <x>\n"
                    "   Задает координату X базовой точки объекта\n"
                    "       (аналогично для Y и Z)\n"
                    " BASE/+x <Имя> <x>\n"
                    "   Задает приращение координаты X базовой точки объекта\n"
                    "       (аналогично для Y и Z)\n"
                    " BASE> <Имя>\n"
                    "   Задает клавиатурное управление базовой точкой объекта\n",
                    &RSS_Module_Missile::cBase },
 { "angle",   "a", "#ANGLE - задать углы ориентации объекта",
                    "           A (AZIMUTH)   - азимут\n"
                    "           E (ELEVATION) - возвышение\n"
                    "           R (ROLL)      - крен\n"
                    " ANGLE <Имя> <a> <e> <r>\n"
                    "   Задает углы ориентации объекта\n"
                    " ANGLE/a <Имя> <a>\n"
                    "   Задает угол ориентации A объекта\n"
                    "       (аналогично для E и R)\n"
                    " ANGLE/+a <Имя> <a>\n"
                    "   Задает приращение угла ориентации A объекта\n"
                    "       (аналогично для E и R)\n"
                    " ANGLE> <Имя>\n"
                    "   Задает клавиатурное управление углами ориентации объекта\n",
                    &RSS_Module_Missile::cAngle },
 { "velocity", "v", "#VELOCITY - задание скорости движения объекта",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задание абсолютного значения скорости объекта"
                    " VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>\n"
                    "   Задание проекций скорости объекта",
                    &RSS_Module_Missile::cVelocity },
 { "control",  "c", "#CONTROL - управление объектом",
                    " CONTROL <Имя> <Угол крена> [<Перегрузка>]\n"
                    "   Задание полного набора параметров управления\n"
                    " CONTROL/G <Имя> <Перегрузка>\n"
                    "   Задание полного набора параметров управления\n"
                    " CONTROL> <Имя> [<Шаг крена> [<Шаг ускорения>]]\n"
                    "   Управление стрелочками\n",
                    &RSS_Module_Missile::cControl },
 { "trace",    "t", "#TRACE - трассировка траектории объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Трассировка траектории объекта в реальном времени\n",
                    &RSS_Module_Missile::cTrace },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Missile_instr *RSS_Module_Missile::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Missile::RSS_Module_Missile(void)

{
	   keyword="EmiStand" ;
    identification="Missile" ;
          category="Object" ;

        mInstrList=RSS_Module_Missile_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Missile::~RSS_Module_Missile(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Missile::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_Missile *object ;
                char  models_list[4096] ;
                char *end ;
                 int  i ;
                 int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define       PAR             object->Parameters
#define       PAR_CNT         object->Parameters_cnt
 
/*--------------------------------------------------- Проверка имени */

    if(data->name[0]==0) {                                           /* Если имя НЕ задано */
              SEND_ERROR("Секция MISSILE: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция MISSILE: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция MISSILE: Не задана модель объекта") ;
                                return(NULL) ;
                          }

        RSS_Model_list(data->lib_path, models_list,                 /* Формирование списка моделей */
                                sizeof(models_list)-1, "BODY" );

        for(end=models_list ; *end ; ) {                            /* Ищем модель по списку */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

           if(*end==0) {
              SEND_ERROR("Секция MISSILE: Неизвестная модель тела") ;
                                return(NULL) ;
                       }

                                    end+=strlen(end)+1 ;            /* Извлекаем имя файла */

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* Считываем параметры модели */
                        }
/*- - - - - - - - - - - - - - - - -  Если модель задана полным путем */
   else                 {
                              RSS_Model_ReadPars(data) ;            /* Считываем параметры модели */
                        }
/*--------------------------------------- Контроль списка параметров */

     for(i=0 ; i<5 ; i++)
       if((data->pars[i].text [0]==0 &&
           data->pars[i].value[0]!=0   ) ||
          (data->pars[i].text [0]!=0 &&
           data->pars[i].value[0]==0   )   ) {

              SEND_ERROR("Секция MISSILE: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_Missile ;
    if(object==NULL) {
              SEND_ERROR("Секция MISSILE: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }
/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция MISSILE: Переполнение памяти") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
            sprintf( PAR[PAR_CNT].name, "PAR%d", i+1) ;
                     PAR[PAR_CNT].value=strtod(data->pars[i].value, &end) ;
                     PAR[PAR_CNT].ptr  = NULL ;
                         PAR_CNT++ ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - Терминируем пустой записью */
           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция MISSILE: Переполнение памяти") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;

/*---------------------------------- Создание списка свойств обьекта */

      object->Features_cnt=this->feature_modules_cnt ;
      object->Features    =(RSS_Feature **)
                             calloc(this->feature_modules_cnt, 
                                     sizeof(object->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++)
      object->Features[i]=this->feature_modules[i]->vCreateFeature(object, NULL) ;

/*-------------------------------------- Считывание описаний свойств */

           RSS_Model_ReadSect(data) ;                               /* Считываем секции описаний модели */

   for(i=0 ; data->sections[i].title[0] ; i++) {

     for(j=0 ; j<object->Features_cnt ; j++) {

          object->Features[j]->vBodyPars(NULL, PAR) ;
          object->Features[j]->vReadSave(data->sections[i].title, 
                                         data->sections[i].decl, "Missile.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция MISSILE: Переполнение памяти") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

       strcpy(object->Name,       data->name) ;
       strcpy(object->model_path, data->path) ;
              object->Module=this ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   PAR
#undef   PAR_CNT

  return(object) ;
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Missile::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Простая ракета с пассивным самонаведением", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Missile::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MISSILE"
#define  _SECTION_SHRT_NAME   "MISSILE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Missile:") ;
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
       if(status)  SEND_ERROR("Секция MISSILE: Неизвестная команда") ;
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

    void  RSS_Module_Missile::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
 RSS_Object_Missile *object ;
               char  name[128] ;
               char *entry ;
               char *end ;
                int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE MISSILE\n", 
                      strlen("#BEGIN MODULE MISSILE\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT MISSILE\n", 
                      strlen("#BEGIN OBJECT MISSILE\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT MISSILE\n", 
              strlen("#BEGIN OBJECT MISSILE\n"))) {                 /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  Извлечение параметров */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* Извлекаем имя объекта */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;

                                     entry=strstr(buff, "MODEL=") ; /* Извлекаем модель объекта */
           strncpy(create_data.path, entry+strlen("MODEL="),
                                       sizeof(create_data.path)-1) ;
        end=strchr(create_data.path, '\n') ;
       *end= 0 ;

    for(i=0 ; i<_MODEL_PARS_MAX ; i++) {
             sprintf(name, "PAR_%d=", i) ;
        entry=strstr(buff, name) ;
     if(entry!=NULL) {
           strncpy(create_data.pars[i].value, entry+strlen(name), 
                                        sizeof(create_data.pars[i].value)-1) ;
        end=strchr(create_data.pars[i].value, '\n') ;
       *end= 0 ;
                     }
                                       } 
/*- - - - - - - - - - - - - - - Проверка повторного создания объекта */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
                object=(RSS_Object_Missile *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - Пропись базовой точки и ориентации объекта */
       entry=strstr(buff, "X_BASE=") ; object->x_base=atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->y_base=atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->z_base=atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->a_azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->a_elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->a_roll=atof(entry+strlen("A_ROLL=")) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
        object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                                  object->y_base, 
                                                  object->z_base ) ;
        object->Features[i]->vBodyAngles   (NULL, object->a_azim, 
                                                  object->a_elev, 
                                                  object->a_roll ) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }                   /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Missile::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE MISSILE\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Missile::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Missile_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_Missile::cCreate(char *cmd)

{
 RSS_Model_data  data ;
     RSS_Object *object ;
           char *name ;
           char *model ;
           char *pars[10] ;
           char *end ;
           char  tmp[1024] ;
        INT_PTR  status ;
            int  i ;

/*-------------------------------------- Дешифровка командной строки */

                             model ="" ;
     for(i=0 ; i<10 ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя объекта */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                 model=end+1 ;                                      /* Извлекаем название модели */
                   end=strchr(model, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<10 ; i++) {                                        /* Извлекаем параметры */
               pars[i]=end+1 ;            
                   end=strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                           }
      } while(0) ;                                                  /* BLOCK.1 */

/*--------------------------------- Подготовка блока данных создания */

         memset(&data, 0, sizeof(data)) ;

        strncpy(data.name,  name,  sizeof(data.name)) ;
        strncpy(data.model, model, sizeof(data.model)) ;

    for(i=0 ; *pars[i]!=0 ; i++) {
        strncpy(data.pars[i].value,  pars[i], sizeof(data.pars[i].value)) ;
         strcpy(data.pars[i].text, "") ;
                                 }

        sprintf(data.lib_path, "%s\\Body.lib", getcwd(tmp, sizeof(tmp))) ;

/*---------------------- Проверка необходимости уточнения параметров */

   if(data.name[0]!=0) {
                            object=vCreateObject(&data) ;
                         if(object!=NULL) {
                                             this->kernel->vShow(NULL) ;
                                                return(0) ;
                                          }
                       }
/*----------------------------------------------- Проведение диалога */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_CREATE", RT_DIALOG),
			             GetActiveWindow(), 
                                     Object_Missile_Create_dialog, 
                                    (LPARAM)&data               ) ;
   if(status)  return(-1) ;

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

  int  RSS_Module_Missile::cInfo(char *cmd)

{
               char  *name ;
 RSS_Object_Missile  *object ;
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

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: INFO <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 8.2lf\r\n" 
                    "     Y % 8.2lf\r\n" 
                    "     Z % 8.2lf\r\n"
                    "Ang. A % 8.2lf\r\n" 
                    "     E % 8.2lf\r\n" 
                    "     R % 8.2lf\r\n"
                    "Vel. V % 8.2lf\r\n" 
                    "     X % 8.2lf\r\n" 
                    "     Y % 8.2lf\r\n" 
                    "     Z % 8.2lf\r\n"
                    "G-ctrl % 8.2lf\r\n" 
                    "Owner  %s\r\n" 
                    "Target %s\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev, object->a_roll,
                        object->v_abs,
                        object->x_velocity, object->y_velocity, object->z_velocity,
                        object->g_ctrl, object->owner, object->target
                    ) ;

           info=text ;

/*----------------------------------------------- Запрос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++) {

                      object->Features[i]->vGetInfo(&f_info) ;
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
/*		      Реализация инструкции OWNER                   */
/*								    */
/*       OWNER <Имя> <Носитель>                                     */

  int  RSS_Module_Missile::cOwner(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *owner ;
 RSS_Object_Missile  *missile ;
         RSS_Object  *object ;
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
                    owner=pars[1] ;   

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: OWNER <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       missile=(RSS_Object_Missile *)FindObject(name, 1) ;          /* Ищем объект-цель по имени */
    if(missile==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(owner==NULL) {                                               /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-носителя. \n"
                                 "Например: OWNER <Имя_ракеты> <Имя_носителя>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* Ищем объект-носитель по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Пропись носителя */

          strcpy(missile->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TARGET                  */
/*								    */
/*       TARGET <Имя> <Цель>                                        */

  int  RSS_Module_Missile::cTarget(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *target ;
 RSS_Object_Missile  *missile ;
         RSS_Object  *object ;
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
                   target=pars[1] ;   

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: TARGET <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       missile=(RSS_Object_Missile *)FindObject(name, 1) ;          /* Ищем объект-цель по имени */
    if(missile==NULL)  return(-1) ;

/*---------------------------------------------- Контроль имени цели */

    if(target==NULL) {                                              /* Если имя не задано... */
                        SEND_ERROR("Не задано имя объекта-цели. \n"
                                   "Например: TARGET <Имя_ракеты> <Имя_цели>") ;
                                     return(-1) ;
                    }

       object=FindObject(target, 0) ;                               /* Ищем объект-цели по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------------- Пропись цели */

               strcpy(missile->target, target) ;

   for(i=0 ; i<missile->Features_cnt ; i++)
     if(!stricmp(missile->Features[i]->Type, "Hit")) 
         strcpy(((RSS_Feature_Hit *)(missile->Features[i]))->target, target) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BASE                    */
/*								    */
/*        BASE    <Имя> <X> <Y> <Z>                                 */
/*        BASE/X  <Имя> <X>                                         */
/*        BASE/+X <Имя> <X>                                         */
/*        BASE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        BASE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Missile::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
                int   xyz_flag ;          /* Флаг режима одной координаты */
                int   delta_flag ;        /* Флаг режима приращений */
                int   arrow_flag ;        /* Флаг стрелочного режима */
               char  *arrows ;
               char  *error ;
               char  *end ;
                int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'x')!=NULL ||
                   strchr(cmd, 'X')!=NULL   )    xyz_flag='X' ;
           else if(strchr(cmd, 'y')!=NULL ||
                   strchr(cmd, 'Y')!=NULL   )    xyz_flag='Y' ;
           else if(strchr(cmd, 'z')!=NULL ||
                   strchr(cmd, 'Z')!=NULL   )    xyz_flag='Z' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='X' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='X' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse=-1. ;  }
           
                          xyz=&pars[2] ;
                   }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: BASE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* Для стрелочного режима берем в качестве шага */
                                                                    /*  самое последнее значение                    */
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='X')   object->x_base+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->y_base+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='X')   object->x_base=coord[0] ;
     else if(xyz_flag=='Y')   object->y_base=coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base=coord[0] ;
     else                   {
                              object->x_base=coord[0] ;
                              object->y_base=coord[1] ;
                              object->z_base=coord[2] ;
                            }
                  }
/*---------------------------------------------- Перенос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                               object->y_base, 
                                               object->z_base ) ;

/*------------------------------------------------------ Отображение */

                      this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ANGLE                   */
/*								    */
/*       ANGLE    <Имя> <A> <E> <R>                                 */
/*       ANGLE/A  <Имя> <A>                                         */
/*       ANGLE/+A <Имя> <A>                                         */
/*       ANGLE>   <Имя> <Код стрелочки> <Шаг>                       */
/*       ANGLE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Missile::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
                int   xyz_flag ;          /* Флаг режима одной координаты */
                int   delta_flag ;        /* Флаг режима приращений */
                int   arrow_flag ;        /* Флаг стрелочного режима */
               char  *arrows ;
           Matrix2d  Sum_Matrix ;
           Matrix2d  Oper_Matrix ;  
           Matrix2d  Velo_Matrix ;  
               char  *error ;
               char  *end ;
                int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }  
           
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ANGLE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* Для стрелочного режима берем в качестве шага */
                                                                    /*  самое последнее значение                    */
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='A')   object->a_azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->a_elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='A')   object->a_azim=coord[0] ;
     else if(xyz_flag=='E')   object->a_elev=coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll=coord[0] ;
     else                   {
                              object->a_azim=coord[0] ;
                              object->a_elev=coord[1] ;
                              object->a_roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(object->a_azim> 180.)  object->a_azim-=360. ;
     while(object->a_azim<-180.)  object->a_azim+=360. ;

     while(object->a_elev> 180.)  object->a_elev-=360. ;
     while(object->a_elev<-180.)  object->a_elev+=360. ;

     while(object->a_roll> 180.)  object->a_roll-=360. ;
     while(object->a_roll<-180.)  object->a_roll+=360. ;

/*---------------------------------------------- Перерасчет скорости */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->a_azim) ;
       Oper_Matrix.Load3d_elev(-object->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*---------------------------------------------- Перенос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->a_azim, 
                                            object->a_elev, 
                                            object->a_roll ) ;

/*------------------------------------------------------ Отображение */

                this->kernel->vShow(NULL) ;

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
/*       VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>      */

  int  RSS_Module_Missile::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
 RSS_Object_Missile  *object ;
               char  *error ;
           Matrix2d  Sum_Matrix ;
           Matrix2d  Oper_Matrix ;  
           Matrix2d  Velo_Matrix ;  
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

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: VELOCITY <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

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
      if(coord_cnt==0)  error="Не указана скорость или ее проекции" ;
      if(coord_cnt==2)  error="Должно быть указаны 3 проекции скорости" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись скорости */
/*- - - - - - - - - - - - - - - - - - - Абсолютное значение скорости */
   if(coord_cnt==1) {
                          object->v_abs=coord[0] ;

                Velo_Matrix.LoadZero   (3, 1) ;
                Velo_Matrix.SetCell    (2, 0, object->v_abs) ;

                 Sum_Matrix.Load3d_azim(-object->a_azim) ;
                Oper_Matrix.Load3d_elev(-object->a_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

                       object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
                       object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
                       object->z_velocity=Velo_Matrix.GetCell(2, 0) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Проекции скорости */
   else             {
                       object->x_velocity=coord[0] ;
                       object->y_velocity=coord[1] ;
                       object->z_velocity=coord[2] ;

                       object->v_abs     =sqrt(object->x_velocity*object->x_velocity+
                                               object->y_velocity*object->y_velocity+
                                               object->z_velocity*object->z_velocity ) ;
                    }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CONTROL                 */
/*								    */
/*       CONTROL   <Имя> <Угол крена> [<Перегрузка>]                */
/*       CONTROL/G <Имя> <Перегрузка>                               */
/*       CONTROL>  <Имя> <Код стрелочки>                            */
/*       CONTROL>> <Имя> <Код стрелочки>                            */

  int  RSS_Module_Missile::cControl(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
                int   xyz_flag ;          /* Флаг режима одной координаты */
                int   g_flag ;            /* Флаг задания перегрузки */
                int   s_flag ;            /* Флаг задания шагов изменения */
                int   arrow_flag ;        /* Флаг стрелочного режима */
               char  *arrows ;
               char  *error ;
               char  *end ;
                int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                          g_flag=0 ;
                          s_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'g')!=NULL ||
                   strchr(cmd, 'G')!=NULL   )  g_flag=1 ;

                           cmd=end+1 ;
                     }

  else if(*cmd=='>') {
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL) {  xyz_flag='R' ;  inverse= 1. ;  }
      if(strstr(arrows, "right")!=NULL) {  xyz_flag='R' ;  inverse=-1. ;  }  

      if(strstr(arrows, "up"   )!=NULL) {  xyz_flag='G' ;  inverse=-1. ;  }
      if(strstr(arrows, "down" )!=NULL) {  xyz_flag='G' ;  inverse= 1. ;  }
           
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: CONTROL <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указана координата или ее приращение" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*----------------------------------- Задание траекторной перегрузки */

   if(g_flag) {
                  object->g_ctrl=coord[0] ;

                      return(0) ;
              }
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(arrow_flag) {

          if(xyz_flag=='G')   object->g_ctrl+=inverse*this->g_step ;
     else if(xyz_flag=='R')   object->a_roll+=inverse*this->a_step ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {
                               object->a_roll=coord[0] ;
              if(coord_cnt>1)  object->g_ctrl=coord[1] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(object->a_roll> 180.)  object->a_roll-=360. ;
     while(object->a_roll<-180.)  object->a_roll+=360. ;

/*---------------------------------------------- Перенос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->a_azim, 
                                            object->a_elev, 
                                            object->a_roll ) ;

/*------------------------------------------------------ Отображение */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TRACE                   */
/*								    */
/*       TRACE <Имя> [<Длительность>]                               */

  int  RSS_Module_Missile::cTrace(char *cmd)

{
#define   _PARS_MAX  10

               char *pars[_PARS_MAX] ;
               char *name ;
             double  trace_time ;
             double  time_0 ;        /* Стартовое время расчета */ 
             double  time_1 ;        /* Текущее время */ 
             double  time_c ;        /* Абсолютное время расчета */ 
             double  time_s ;        /* Последнее время отрисовки */ 
             double  time_w ;        /* Время ожидания */ 
 RSS_Object_Missile *object ;
           Matrix2d  Sum_Matrix ;
           Matrix2d  Oper_Matrix ;  
           Matrix2d  Velo_Matrix ;  
               char *end ;
                int  i ;

/*---------------------------------------- Разборка командной строки */

                     trace_time=0. ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }


                     name=pars[0] ;

           if( pars[1]!=NULL &&
              *pars[1]!=  0    ) {
                                      trace_time=strtod(pars[1], &end) ;
                                   if(trace_time<=0.) {
                                         SEND_ERROR("Задано некорректное время трассировки") ;
                                                           return(-1) ;
                                                      }
                                 }
           else                  {
                                       trace_time=60. ;
                                         SEND_ERROR("Время трассировки - 60 секунд") ;
                                 }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: TRACE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------- Контроль носителя и цели */

       object->o_owner=FindObject(object->owner, 0) ;               /* Ищем носитель по имени */
    if(object->o_owner==NULL)  return(-1) ;

  if(object->target[0]!=0) {
       object->o_target=FindObject(object->target, 0) ;             /* Ищем цель по имени */
    if(object->o_target==NULL)  return(-1) ;
                           }
/*------------------------------ Привязка стартовой точки к носителю */

       object->x_base=object->o_owner->x_base ;
       object->y_base=object->o_owner->y_base ;
       object->z_base=object->o_owner->z_base ;

       object->a_azim=object->o_owner->a_azim ;
       object->a_elev=object->o_owner->a_elev ;
       object->a_roll=object->o_owner->a_roll ;

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->a_azim) ;
       Oper_Matrix.Load3d_elev(-object->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*------------------------------------------------------ Трассировка */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

         object->iSaveTracePoint("CLEAR") ;

    do {                                                            /* CIRCLE.1 - Цикл трассировки */
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отработка времени */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

           if(time_1-time_0>trace_time)  break ;                    /* Если время трассировки закончилось */

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - - - - Моделирование движения */
         object->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->iSaveTracePoint("ADD") ;
/*- - - - - - - - - - - - - - - - - - - - - - Отображение траектории */
         object->iShowTrace_() ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Отображение объекта */
   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                               object->y_base, 
                                               object->z_base ) ;
     object->Features[i]->vBodyAngles   (NULL, object->a_azim, 
                                               object->a_elev, 
                                               object->a_roll ) ;
                                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка сцены */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;                                                 /* END CIRCLE.1 - Цикл трассировки */

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа MISSILE по имени              */

  RSS_Object *RSS_Module_Missile::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Missile")) {

           SEND_ERROR("Объект не является объектом типа MISSILE") ;
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
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "РАКЕТА"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_Missile::RSS_Object_Missile(void)

{
   strcpy(Type, "Missile") ;

    Context        =new RSS_Transit_Missile ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     battle_state= 0 ;

      x_base    =   0. ;
      y_base    =   0. ;
      z_base    =   0. ;
      a_azim    =   0. ;
      a_elev    =   0. ;
      a_roll    =   0. ;
      x_velocity=   0. ;
      y_velocity=   0. ;
      z_velocity=   0. ;
      v_abs     =   0. ;
      g_ctrl    = 100. ;

      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=RGB(0, 0, 127) ;
      mTrace_dlist=  0 ;  
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Missile::~RSS_Object_Missile(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Object_Missile::vFree(void)

{
  int  i ;


  if(this->mTrace!=NULL) {
                             free(this->mTrace) ;
                                  this->mTrace    =NULL ;
                                  this->mTrace_cnt=  0 ;  
                                  this->mTrace_max=  0 ;  
                         }

  if(this->Features!=NULL) {

    for(i=0 ; i<this->Features_cnt ; i++) {
                this->Features[i]->vBodyDelete(NULL) ;
           free(this->Features[i]) ;
                                         }

           free(this->Features) ;

                this->Features    =NULL ;
                this->Features_cnt=  0 ;
                           }
}


/********************************************************************/
/*								    */
/*                    Сохранить состояние объекта                   */
/*                    Восстановить состояние объекта                */

    void  RSS_Object_Missile::vPush(void)

{
     x_base_stack    =x_base ;
     y_base_stack    =y_base ;
     z_base_stack    =z_base ;

     a_azim_stack    =a_azim ;
     a_elev_stack    =a_elev ;
     a_roll_stack    =a_roll ;

     x_velocity_stack=x_velocity ;
     y_velocity_stack=y_velocity ;
     z_velocity_stack=z_velocity ;
}


    void  RSS_Object_Missile::vPop(void)

{
     x_base    =x_base_stack ;
     y_base    =y_base_stack ;
     z_base    =z_base_stack ;

     a_azim    =a_azim_stack ;
     a_elev    =a_elev_stack ;
     a_roll    =a_roll_stack ;

     x_velocity=x_velocity_stack ;
     y_velocity=y_velocity_stack ;
     z_velocity=z_velocity_stack ;

  if(this->mTrace!=NULL) {
                             free(this->mTrace) ;
                                  this->mTrace    =NULL ;
                                  this->mTrace_cnt=  0 ;  
                                  this->mTrace_max=  0 ;  

                                      iShowTrace_() ;
                         }
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_Missile::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT MISSILE\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n",       this->Name      ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->x_base    ) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->y_base    ) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->z_base    ) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->a_azim    ) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->a_elev    ) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->a_roll    ) ;  *text+=field ;
    sprintf(field, "MODEL=%s\n",      this->model_path) ;  *text+=field ;

  for(i=0 ; i<this->Parameters_cnt ; i++) {
    sprintf(field, "PAR_%d=%.10lf\n", 
                          i, this->Parameters[i].value) ;  *text+=field ;
                                          }
/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        Обработка событий                         */

     int  RSS_Object_Missile::vEvent(char *event_name, double  t)
{
//    RSS_Feature_Hit *hit ; 

/*--------------------------------------------------- Поражение цели */

   if(!stricmp(event_name, "HIT")) {
                                         return(0) ;
                                   }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Object_Missile::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_Missile::vCalculateStart(double  t)
{
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;  
    Matrix2d  Velo_Matrix ;  
         int  i ; 

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*-------------------------------------- Привязка к объекту-носителю */

      this->o_owner=NULL ;

  if(this->owner[0]!=0) {

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, this->owner)) {
                       this->o_owner=OBJECTS[i] ;
                                 break ;
                                                     }
                        }

  if(this->o_owner!=NULL) {

      this->x_base=this->o_owner->x_base ;
      this->y_base=this->o_owner->y_base ;
      this->z_base=this->o_owner->z_base ;

      this->a_azim=this->o_owner->a_azim ;
      this->a_elev=this->o_owner->a_elev ;
      this->a_roll=this->o_owner->a_roll ;
                          }
/*------------------------------------------ Привязка к объекту-цели */

      this->o_target=NULL ;

  if(this->target[0]!=0) {

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, this->target)) {
                       this->o_target=OBJECTS[i] ;
                                 break ;
                                                      }
                         }
/*-------------------------------------------- Перерасчет параметров */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, this->v_abs) ;
        Sum_Matrix.Load3d_azim(-this->a_azim) ;
       Oper_Matrix.Load3d_elev(-this->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         this->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         this->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         this->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*------------------------------------------------ Очистка контекста */

    this->iSaveTracePoint("CLEAR") ;

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_Missile::vCalculate(double t1, double t2, char *callback, int cb_size)
{
    double  dx, dy, dz ;         /* Вектор на цель */
    double  s ;                  /* Расстояние до цели */
    double  r ;                  /* Радиус маневра */
    double  h ;
    double  ds ;
    double  a ;                  /* Ометываемый угол маневра */
    double  b ;
    double  dv_max ;             /* Максимальная возможная длина вектора изменение скорости */
    double  dv_x, dv_y, dv_z ;   /* Требуемое изменение скорости */
    double  dv_s ;               /* Длина вектора требуемого изменения скорости */
    double  pv_x, pv_y, pv_z ;   /* Предыдущий вектор скорости  */

/*------------------------------------------------------- Подготовка */

            pv_x=x_velocity ;
            pv_y=y_velocity ;
            pv_z=z_velocity ;

/*---------------------------------------------------- Без наведения */

   if(this->o_target==NULL) {
                               x_base+=x_velocity*(t2-t1) ;
                               y_base+=y_velocity*(t2-t1) ;
                               z_base+=z_velocity*(t2-t1) ;
                               g_over = 0 ;
                            }
/*----------------------------------------------------- С наведением */

   else                     {
/*- - - - - - - - - - - - - - Параметры относительного движения цели */
                       dx=o_target->x_base-x_base ;                 /* Вектор на цель */
                       dy=o_target->y_base-y_base ;
                       dz=o_target->z_base-z_base ;
                        s=sqrt(dx*dx+dy*dy+dz*dz) ;
/*- - - - - - - - - -  Расчет предельного изменения вектора скорости */
                        r= v_abs*v_abs/g_ctrl ;                     /* Максимальный вектор изменения */
                        a= v_abs*(t2-t1)/(2.*r) ;
                   dv_max= 2.*v_abs*sin(0.5*a) ;
/*- - - - - - - - - - - Расчет требуемого изменения вектора скорости */
                       dx=dx*v_abs/s ;                              /* Нормируем вектор на цель по скорости */
                       dy=dy*v_abs/s ;
                       dz=dz*v_abs/s ;

                     dv_x=dx-x_velocity ;                           /* Требуемый вектор изменения скорости */
                     dv_y=dy-y_velocity ;
                     dv_z=dz-z_velocity ;
                     dv_s=sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
/*- - - - - - - - - -  Проверка ухода из поля видимости 180 градусов */
     if(dv_s>1.4*v_abs) {
                               x_base+=x_velocity*(t2-t1) ;
                               y_base+=y_velocity*(t2-t1) ;
                               z_base+=z_velocity*(t2-t1) ;
                               g_over = 0 ;

                                   return(0) ;
                        }
/*- - - - - - - - - - - - - - - - - - Расчет нового вектора скорости */
     if(dv_s>dv_max) {                                              /* Если требуемое изменение не может быть обеспечено по перегрузке... */
                            b=asin(0.5*dv_s/v_abs) ;
                            h=sqrt(v_abs*v_abs-0.25*dv_s*dv_s) ;

                           ds=h*tan(a-b)+0.5*dv_s ;

                         x_velocity+=dv_x*ds/dv_s ;
                         y_velocity+=dv_y*ds/dv_s ;
                         z_velocity+=dv_z*ds/dv_s ;
                               dv_s =sqrt(x_velocity*x_velocity+
                                          y_velocity*y_velocity+
                                          z_velocity*z_velocity ) ;
                         x_velocity*=v_abs/dv_s ;
                         y_velocity*=v_abs/dv_s ;
                         z_velocity*=v_abs/dv_s ;
                             g_over = 1 ;
                     }
     else            {
                         x_velocity =dx ;
                         y_velocity =dy ;
                         z_velocity =dz ;
                             g_over = 0 ;
                     }
/*- - - - - - - - - - - - - - - - - - - - -  Изменение базовой точки */
             x_base+=0.5*(x_velocity+pv_x)*(t2-t1) ;
             y_base+=0.5*(y_velocity+pv_y)*(t2-t1) ;
             z_base+=0.5*(z_velocity+pv_z)*(t2-t1) ;

                  s=sqrt((x_velocity-pv_x)*(x_velocity-pv_x)+
                         (y_velocity-pv_y)*(y_velocity-pv_y)+
                         (z_velocity-pv_z)*(z_velocity-pv_z) ) ;
/*- - - - - - - - - - - - - - - - - - - - Изменение углов ориентации */
                  a_azim=atan2(x_velocity, z_velocity)*_RAD_TO_GRD ;
                  a_elev=atan2(y_velocity, sqrt(x_velocity*x_velocity+z_velocity*z_velocity))*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_Missile::vCalculateShow(double  t1, double  t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* Сохранение точки траектории */  

         this->iShowTrace_() ;                                      /* Отображение траектории */

   for(i=0 ; i<this->Features_cnt ; i++) {                          /* Отображение объекта */
     this->Features[i]->vBodyBasePoint(NULL, this->x_base, 
                                             this->y_base, 
                                             this->z_base ) ;
     this->Features[i]->vBodyAngles   (NULL, this->a_azim, 
                                             this->a_elev, 
                                             this->a_roll ) ;
                                         }

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   Сохранение точки траектории                     */

  int  RSS_Object_Missile::iSaveTracePoint(char *action)

{
/*------------------------------------------------- Сброс траектории */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- Довыделение буфера */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_MissileTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_MissileTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("MISSILE.iSaveTracePoint@"
                              "Memory over for trajectory") ;
                                  return(-1) ;
                        }
                              }
/*------------------------------------------------- Сохранение точки */
                  
                  mTrace[mTrace_cnt].x_base    =this->x_base ;
                  mTrace[mTrace_cnt].y_base    =this->y_base ;
                  mTrace[mTrace_cnt].z_base    =this->z_base ;
                  mTrace[mTrace_cnt].a_azim    =this->a_azim ;
                  mTrace[mTrace_cnt].a_elev    =this->a_elev ;
                  mTrace[mTrace_cnt].a_roll    =this->a_roll ;
                  mTrace[mTrace_cnt].x_velocity=this->x_velocity ;
                  mTrace[mTrace_cnt].y_velocity=this->y_velocity ;
                  mTrace[mTrace_cnt].z_velocity=this->z_velocity ;

      if(g_over)  mTrace[mTrace_cnt].color     =RGB(127, 0,   0) ;
      else        mTrace[mTrace_cnt].color     =RGB(  0, 0, 127) ;

                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           Отображение траектории с передачей контекста           */

  void  RSS_Object_Missile::iShowTrace_(void)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*								     */
/*                     Отображение траектории                        */

  void  RSS_Object_Missile::iShowTrace(void)

{
       int  status ;
       int  i ;

/*-------------------------------- Резервирование дисплейного списка */

     if(mTrace_dlist==0) {
           mTrace_dlist=RSS_Kernel::display.GetList(2) ;
                         }

     if(mTrace_dlist==0)  return ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- Настройка контекста отображения */

             glNewList(mTrace_dlist, GL_COMPILE) ;                  /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;

/*--------------------------------------------- Отрисовка траектории */

    if(mTrace_cnt>0) {
                                        i=0 ;

             glColor4d(GetRValue(mTrace[i].color)/256., 
                       GetGValue(mTrace[i].color)/256.,
                       GetBValue(mTrace[i].color)/256., 1.) ;

               glBegin(GL_LINE_STRIP) ;

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;

       for(i=1 ; i<mTrace_cnt ; i++) {

         if(mTrace[i].color!=mTrace[i-1].color) {

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;
                 glEnd();
             glColor4d(GetRValue(mTrace[i].color)/256., 
                       GetGValue(mTrace[i].color)/256.,
                       GetBValue(mTrace[i].color)/256., 1.) ;

               glBegin(GL_LINE_STRIP) ;
                                                }

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;

                                     }

                         glEnd();

                     }
/*----------------------------- Восстановление контекста отображения */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit_Missile::RSS_Transit_Missile(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Missile::~RSS_Transit_Missile(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Missile::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_Missile *)object)->iShowTrace() ;

   return(0) ;
}

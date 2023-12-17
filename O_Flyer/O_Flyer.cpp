/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЛЕТУН"  		    */
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

#include "O_Flyer.h"

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

     static   RSS_Module_Flyer  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_FLYER_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_FLYER_API RSS_Kernel *GetEntry(void)

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

  struct RSS_Module_Flyer_instr  RSS_Module_Flyer_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Flyer::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_Flyer::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_Flyer::cInfo },
 { "copy",    "cp", "#COPY - копировать объект",
                    " COPY <Имя образца> <Имя нового объекта>\n"
                    "   Копировать объект",
                    &RSS_Module_Flyer::cCopy },
 { "owner",   "o",  "#OWNER - назначить носитель",
                    " OWNER <Имя obъекта> <Носитель>\n"
                    "   Назначить объект - носитель",
                    &RSS_Module_Flyer::cOwner },
 { "target",  "tg", "#TARGET - назначить цель",
                    " TARGET <Имя> <Цель>\n"
                    "   Назначить объект - цель",
                    &RSS_Module_Flyer::cTarget },
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
                    &RSS_Module_Flyer::cBase },
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
                    &RSS_Module_Flyer::cAngle },
 { "velocity", "v", "#VELOCITY - задание скорости движения объекта",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задание абсолютного значения скорости объекта"
                    " VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>\n"
                    "   Задание проекций скорости объекта",
                    &RSS_Module_Flyer::cVelocity },
 { "control",  "c", "#CONTROL - управление объектом",
                    " CONTROL <Имя> <Угол крена> [<Перегрузка>]\n"
                    "   Задание полного набора параметров управления\n"
                    " CONTROL/G <Имя> <Перегрузка>\n"
                    "   Задание перегрузки маневра\n"
                    " CONTROL> <Имя> [<Шаг крена> [<Шаг ускорения>]]\n"
                    "   Управление стрелочками\n",
                    &RSS_Module_Flyer::cControl },
 { "program",  "p", "#PROGRAM - задание программы управления объектом",
                    " PROGRAM <Имя> <Имя файла программы>\n"
                    "   Задание программы управления объектом\n",
                    &RSS_Module_Flyer::cProgram },
 { "event",    "e", "#EVENT - привязка события к программе управления объектом ",
                    " EVENT <Имя> <Имя события> <Имя программы>\n"
                    "   Привязка события к программе управления объектом\n",
                    &RSS_Module_Flyer::cEvent },
 { "unit",     "u", "#UNIT - добавить компонент в состав объекта",
                    " UNIT <Имя>\n"
                    "   просмотреть список компонентов, добавить компонент в диалоговом режиме\n"
                    " UNIT <Имя объекта> <Имя компонента> <Тип компонента>\n"
                    "   добавить компонент в состав объекта",
                    &RSS_Module_Flyer::cUnit },
 { "path",  "path", "#PATH - включение/выключение отображения траектории движения",
                    " PATH <Имя> <0 или 1>\n"
                    "   Включить (1) или выключить (0) отображения траектории движения\n",
                    &RSS_Module_Flyer::cPath },
 { "trace",    "t", "#TRACE - моделирование движения объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Моделирование движения объекта в реальном времени\n"
                    " TRACE/P <Имя> <Имя программа> [<Длительность>]\n"
                    "   Моделирование движения объекта для программы управления\n"
                    " TRACE/C <Имя> <R> <G> <B>\n"
                    "   Задание цвета трассы\n",
                    &RSS_Module_Flyer::cTrace },
 { "stream",  "st", "#STREAM - задание файла потока телеметрии",
                    " STREAM <Имя> <Имя файла телеметрии>\n"
                    "   Задание файла потока телеметрии\n",
                    &RSS_Module_Flyer::cStream },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Flyer_instr *RSS_Module_Flyer::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Flyer::RSS_Module_Flyer(void)

{
	   keyword="EmiStand" ;
    identification="Flyer" ;
          category="Object" ;

        mInstrList=RSS_Module_Flyer_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Flyer::~RSS_Module_Flyer(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Flyer::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_Flyer *object ;
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
              SEND_ERROR("Секция FLYER: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция FLYER: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция FLYER: Не задана модель объекта") ;
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
              SEND_ERROR("Секция FLYER: Неизвестная модель тела") ;
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

              SEND_ERROR("Секция FLYER: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_Flyer ;
    if(object==NULL) {
              SEND_ERROR("Секция FLYER: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }
/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция FLYER: Переполнение памяти") ;
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
                         SEND_ERROR("Секция FLYER: Переполнение памяти") ;
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
                                         data->sections[i].decl, "Flyer.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция FLYER: Переполнение памяти") ;
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

     int  RSS_Module_Flyer::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Простой летательный аппарат", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Flyer::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "FLYER"
#define  _SECTION_SHRT_NAME   "FLYER"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Flyer:") ;
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
       if(status)  SEND_ERROR("Секция FLYER: Неизвестная команда") ;
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

    void  RSS_Module_Flyer::vReadSave(std::string *data)

{
             char *buff ;
              int  buff_size ;
   RSS_Model_data  create_data ;
 RSS_Object_Flyer *object ;
             char  name[128] ;
//            int  status ;
             char *entry ;
             char *end ;
              int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE FLYER\n", 
                      strlen("#BEGIN MODULE FLYER\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT FLYER\n", 
                      strlen("#BEGIN OBJECT FLYER\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT FLYER\n", 
              strlen("#BEGIN OBJECT FLYER\n"))) {                   /* IF.1 */
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
                object=(RSS_Object_Flyer *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - Пропись базовой точки и ориентации объекта */
       entry=strstr(buff, "X_BASE=") ; object->state.x   =atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->state.y   =atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->state.z   =atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->state.azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->state.elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->state.roll=atof(entry+strlen("A_ROLL=")) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
        object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                                  object->state.y, 
                                                  object->state.z ) ;
        object->Features[i]->vBodyAngles   (NULL, object->state.azim, 
                                                  object->state.elev, 
                                                  object->state.roll ) ;
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

    void  RSS_Module_Flyer::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE FLYER\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Flyer::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Flyer_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_Flyer::cCreate(char *cmd)

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
                                     Object_Flyer_Create_dialog, 
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

  int  RSS_Module_Flyer::cInfo(char *cmd)

{
             char  *name ;
 RSS_Object_Flyer  *object ;
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

       object=FindObject(name, 1) ;                                    /* Ищем объект по имени */
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
                    "\r\n",
                        object->Name, object->Type, 
                        object->state.x, object->state.y, object->state.z,
                        object->state.azim, object->state.elev, object->state.roll,
                        object->v_abs,
                        object->state.x_velocity, object->state.y_velocity, object->state.z_velocity,
                        object->g_ctrl
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
/*		      Реализация инструкции COPY                    */
/*								    */
/*       COPY <Имя образца> <Имя нового объекта>                    */

  int  RSS_Module_Flyer::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                   char  *pars[_PARS_MAX] ;
                   char  *name ;
                   char  *copy ;
       RSS_Object_Flyer  *flyer ;
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
                     copy=pars[1] ;   

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: COPY <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       flyer=(RSS_Object_Flyer *)FindObject(name, 1) ;              /* Ищем объект-образец по имени */
    if(flyer==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(copy==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-копии. \n"
                                 "Например: COPY <Имя образца> <Имя нового объекта>") ;
                                     return(-1) ;
                   }

       object=FindObject(copy, 0) ;                                 /* Ищем объект-копию по имени */
    if(object!=NULL) {
                       SEND_ERROR("Oбъект-копия уже существует") ;
                                     return(-1) ;
                     }
/*---------------------------------------------- Копирование объекта */

            object=flyer->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции OWNER                   */
/*								    */
/*       OWNER <Имя> <Носитель>                                     */

  int  RSS_Module_Flyer::cOwner(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *owner ;
   RSS_Object_Flyer  *flyer ;
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

       flyer=(RSS_Object_Flyer *)FindObject(name, 1) ;              /* Ищем объект-цель по имени */
    if(flyer==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(owner==NULL) {                                               /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-носителя. \n"
                                 "Например: OWNER <Имя_объекта> <Имя_носителя>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* Ищем объект-носитель по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Пропись носителя */

          strcpy(flyer->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TARGET                  */
/*								    */
/*       TARGET <Имя> <Цель>                                        */

  int  RSS_Module_Flyer::cTarget(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *target ;
   RSS_Object_Flyer  *flyer ;
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

       flyer=(RSS_Object_Flyer *)FindObject(name, 1) ;              /* Ищем объект по имени */
    if(flyer==NULL)  return(-1) ;

/*---------------------------------------------- Контроль имени цели */

    if(target==NULL) {                                              /* Если имя не задано... */
                        SEND_ERROR("Не задано имя объекта-цели. \n"
                                   "Например: TARGET <Имя_объекта> <Имя_цели>") ;
                                     return(-1) ;
                    }

       object=FindObject(target, 0) ;                               /* Ищем объект-цели по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------------- Пропись цели */

               strcpy(flyer->target, target) ;

   for(i=0 ; i<flyer->Features_cnt ; i++)
     if(!stricmp(flyer->Features[i]->Type, "Hit")) 
         strcpy(((RSS_Feature_Hit *)(flyer->Features[i]))->target, target) ;

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

  int  RSS_Module_Flyer::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
 RSS_Object_Flyer  *object ;
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

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
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

          if(xyz_flag=='X')   object->state.x+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->state.y+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->state.z+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='X')   object->state.x=coord[0] ;
     else if(xyz_flag=='Y')   object->state.y=coord[0] ;                 
     else if(xyz_flag=='Z')   object->state.z=coord[0] ;
     else                   {
                              object->state.x=coord[0] ;
                              object->state.y=coord[1] ;
                              object->state.z=coord[2] ;
                            }
                  }
/*---------------------------------------------- Перенос на Свойства */

         object->vPrepareFeatures(NULL) ;
         object->vCheckFeatures  (NULL, NULL) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                               object->state.y, 
                                               object->state.z ) ;
     object->Features[i]->vBodyAngles   (NULL, object->state.azim, 
                                               object->state.elev, 
                                               object->state.roll ) ;
     object->Features[i]->vShow         (NULL) ;
                                           }
/*------------------------------------------------------ Отображение */

                      this->kernel->vShow(NULL) ;

            object->state_0=object->state ;

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

  int  RSS_Module_Flyer::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
 RSS_Object_Flyer  *object ;
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

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
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

          if(xyz_flag=='A')   object->state.azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->state.elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   object->state.roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='A')   object->state.azim=coord[0] ;
     else if(xyz_flag=='E')   object->state.elev=coord[0] ;                 
     else if(xyz_flag=='R')   object->state.roll=coord[0] ;
     else                   {
                              object->state.azim=coord[0] ;
                              object->state.elev=coord[1] ;
                              object->state.roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(object->state.azim> 180.)  object->state.azim-=360. ;
     while(object->state.azim<-180.)  object->state.azim+=360. ;

     while(object->state.elev> 180.)  object->state.elev-=360. ;
     while(object->state.elev<-180.)  object->state.elev+=360. ;

     while(object->state.roll> 180.)  object->state.roll-=360. ;
     while(object->state.roll<-180.)  object->state.roll+=360. ;

/*---------------------------------------------- Перерасчет скорости */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->state.azim) ;
       Oper_Matrix.Load3d_elev(-object->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*---------------------------------------------- Перенос на Свойства */

         object->vPrepareFeatures(NULL) ;
         object->vCheckFeatures  (NULL, NULL) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyAngles(NULL, object->state.azim, 
                                            object->state.elev, 
                                            object->state.roll ) ;
     object->Features[i]->vShow      (NULL) ;

                                           }
/*------------------------------------------------------ Отображение */

                this->kernel->vShow(NULL) ;

            object->state_0=object->state ;

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

  int  RSS_Module_Flyer::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
 RSS_Object_Flyer  *object ;
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

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
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

                 Sum_Matrix.Load3d_azim(-object->state.azim) ;
                Oper_Matrix.Load3d_elev(-object->state.elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

                       object->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
                       object->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
                       object->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Проекции скорости */
   else             {
                       object->state.x_velocity=coord[0] ;
                       object->state.y_velocity=coord[1] ;
                       object->state.z_velocity=coord[2] ;

                       object->v_abs     =sqrt(object->state.x_velocity*object->state.x_velocity+
                                               object->state.y_velocity*object->state.y_velocity+
                                               object->state.z_velocity*object->state.z_velocity ) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            object->state_0=object->state ;

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

  int  RSS_Module_Flyer::cControl(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
 RSS_Object_Flyer  *object ;
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

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
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

          if(xyz_flag=='G')   object->g_ctrl    +=inverse*this->g_step ;
     else if(xyz_flag=='R')   object->state.roll+=inverse*this->a_step ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {
                               object->state.roll=coord[0] ;
              if(coord_cnt>1)  object->g_ctrl    =coord[1] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(object->state.roll> 180.)  object->state.roll-=360. ;
     while(object->state.roll<-180.)  object->state.roll+=360. ;

            object->state_0=object->state ;

/*---------------------------------------------- Перенос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->state.azim, 
                                            object->state.elev, 
                                            object->state.roll ) ;

/*------------------------------------------------------ Отображение */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PROGRAM                 */
/*								    */
/*      PROGRAM <Имя> <Файл программы>                              */

  int  RSS_Module_Flyer::cProgram(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта.\n"
                                      "Например: PROGRAM <Имя> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------- Считывание данных из файла */

  if(pars[1]==NULL) {
                       SEND_ERROR("Не задано имя файла программы.\n"
                                  "Например: PROGRAM <Имя> <файл программы>") ;
                                     return(-1) ;
                    }

      status=iReadProgram(object, pars[1]) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции EVENT                   */
/*								    */
/*       EVENT <Имя> <Событие> <Программа>                          */

  int  RSS_Module_Flyer::cEvent(char *cmd)

{
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char  *event_name ;
             char  *program ;
 RSS_Object_Flyer  *object ;
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

           if( pars[0]==NULL ||
              *pars[0]==  0    ) {
                                     SEND_ERROR("Не задан объект управления") ;
                                         return(-1) ;
                                 }
           if( pars[1]==NULL ||
              *pars[1]==  0    ) {
                                     SEND_ERROR("Не задано событие") ;
                                         return(-1) ;
                                 }
           if( pars[2]==NULL ||
              *pars[2]==  0    ) {
                                     SEND_ERROR("Не задана программа управления") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
               event_name=pars[1] ;
                  program=pars[2] ;

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: EVENT <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------ Контроль программы управления */

        for(i=0 ; i<_PROGRAMS_MAX ; i++)
          if(object->programs[i]!=NULL)
           if(!stricmp(object->programs[i]->name, program))   break ;

      if(i>=_PROGRAMS_MAX) {
            SEND_ERROR("Неизвестная программа - воспользуйтесь командой PROGRAM для загрузки файла программы") ;
                                     return(-1) ;
                           }
/*---------------------------------------------- Регистрация события */

        for(i=0 ; i<_EVENTS_MAX ; i++)                              /* Проверяем, нет ли уже такого события */
          if(!stricmp(object->events[i].name, event_name))  break ; 

   if(i>=_EVENTS_MAX) {                                             /* Если такого события нет - */
        for(i=0 ; i<_EVENTS_MAX ; i++)                              /*  - ищем пустой слот       */
          if(object->events[i].name[0]==0)  break ; 
                      } 

   if(i>=_EVENTS_MAX) {
                           SEND_ERROR("Переполнение списка событий") ;
                                     return(-1) ;
                      }

        strncpy(object->events[i].name,    event_name, sizeof(object->events[i].name   )-1) ;
        strncpy(object->events[i].program, program,    sizeof(object->events[i].program)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции UNIT                    */
/*								    */
/*     UNIT <Имя объекта>                                           */
/*     UNIT <Имя объекта> <Имя компонента> <Тип компонента>         */

  int  RSS_Module_Flyer::cUnit(char *cmd)

{
#define   _PARS_MAX  10

             char *pars[_PARS_MAX] ;
             char *name ;
             char *unit_name ;
             char *unit_type ;
 RSS_Object_Flyer *object ;
       RSS_Object *unit ;
          INT_PTR  status ;
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
                unit_name=pars[1] ;
                unit_type=pars[2] ;

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: UNIT <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

   if(unit_name==NULL ||
      unit_type==NULL   ) {

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_UNITS", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_Flyer_Units_dialog, 
                                      (LPARAM)object               ) ;
         return((int)status) ;

                          }
/*-------------------------------- Создание и регистрация компонента */

         unit=AddUnit(object, unit_name, unit_type, error) ;        
      if(unit==NULL) {
                       SEND_ERROR(error) ;
                             return(-1) ;
                     }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PATH                    */
/*								    */
/*      PATH <Имя> 0                                                */
/*      PATH <Имя> 1                                                */

  int  RSS_Module_Flyer::cPath(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта.\n"
                                      "Например: PATH <Имя> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Установка значения */

  if(pars[1]==NULL) {
                       SEND_ERROR("Не задано значение флага отображения траектории.\n"
                                  "Например: PATH <Имя> 0") ;
                                     return(-1) ;
                    }

      if(!stricmp(pars[1], "0"))  object->mTrace_flag=0 ;
      else                        object->mTrace_flag=1 ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TRACE                   */
/*								    */
/*       TRACE <Имя> [<Длительность>]                               */
/*       TRACE/P <Имя> <Программа>                                  */
/*       TRACE/C <Имя> <R> <G> <B>                                  */

  int  RSS_Module_Flyer::cTrace(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

             char  *pars[_PARS_MAX] ;
             char  *name ;
           double   trace_time ;
           double   time_0 ;        /* Стартовое время расчета */ 
           double   time_1 ;        /* Текущее время */ 
           double   time_c ;        /* Абсолютное время расчета */ 
           double   time_s ;        /* Последнее время отрисовки */ 
           double   time_w ;        /* Время ожидания */ 
 RSS_Object_Flyer  *object ;
              int   c_flag ;
              int   program_flag ;  /* Режим программного управления */ 
              int   color_r, color_g, color_b ;
             char  *end ;
              int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */

                     trace_time=0. ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                   program_flag=0 ;
                         c_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'p')!=NULL ||
                   strchr(cmd, 'P')!=NULL   )  program_flag=1 ;

                if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  c_flag=1 ;

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


                     name=pars[0] ;

      if(program_flag) {

           if( pars[1]==NULL ||
              *pars[1]==  0    ) {
                                     SEND_ERROR("не задана программа управления") ;
                                         return(-1) ;
                                 }
                       }
      else
      if(c_flag)       {
                       }
      else             {

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
                       }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: TRACE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Если задание цвета */

   if(c_flag) {

     if(pars[3]==NULL) {
                          SEND_ERROR("Формат команды: TRACE/C <Имя> <R> <G> <B>") ;
                                       return(-1) ;
                       }

        color_r=strtoul(pars[1], &end, 10) ;
     if(*end!=0 || color_r>255) {
                     SEND_ERROR("Компонент цвета <R> должен быть числом 0...255") ;
                                       return(-1) ;
                                }

        color_g=strtoul(pars[2], &end, 10) ;
     if(*end!=0 || color_g>255) {
                     SEND_ERROR("Компонент цвета <G> должен быть числом 0...255") ;
                                       return(-1) ;
                                }

        color_b=strtoul(pars[3], &end, 10) ;
     if(*end!=0 || color_b>255) {
                     SEND_ERROR("Компонент цвета <B> должен быть числом 0...255") ;
                                       return(-1) ;
                                }

          object->mTrace_color=RGB(color_r, color_g, color_b) ;

                                       return(0) ;
              }
/*------------------------------------ Контроль программы управления */

                  object->program=NULL ;

    if(program_flag) {

        for(i=0 ; i<_PROGRAMS_MAX ; i++)
          if(object->programs[i]!=NULL)
           if(!stricmp(object->programs[i]->name, pars[1])) {
                    object->program=object->programs[i] ;
                             break ;
                                                            } 

      if(object->program==NULL) {
            SEND_ERROR("Неизвестная программа - воспользуйтесь командой PROGRAM для загрузки файла программы") ;
                                     return(-1) ;
                                }

                memset(&object->p_controls, 0, sizeof(object->p_controls)) ;
                strcpy( object->p_controls.used, ";") ;
                        object->p_frame=0 ;
                        object->p_start=0. ;
                     }
/*----------------------------------- Изменение программы управления */

    if(object->trace_on) {

     if(!program_flag) {
            SEND_ERROR("Объект уже в режиме трассировки") ;
                                     return(-1) ;
                       }

                  object->p_start=object->trace_time ;

                                 return(0) ;
                         }
/*------------------------------------------------------ Трассировка */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

      for(i=0 ; i<OBJECTS_CNT ; i++) {                              /* Инициализация свойств объекта */ 
                    OBJECTS[i]->vResetFeatures  (NULL) ;
                    OBJECTS[i]->vPrepareFeatures(NULL) ;
                                     }

         object->trace_on  =1 ;
         object->trace_time=0. ;

         object->iSaveTracePoint("CLEAR") ;

    do {                                                            /* CIRCLE.1 - Цикл трассировки */
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отработка времени */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

         if(object->program==NULL)
           if(time_1-time_0>trace_time)  break ;                    /* Если время трассировки закончилось */

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
           if(time_w>=0)  Sleep(time_w*1000.) ;
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - - - - Моделирование движения */
      if(object->o_target!=NULL)  object->o_target->state_0=object->o_target->state ; 
                                  object          ->state_0=object          ->state ; 

         object->vCalculate      (time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->vPrepareFeatures(NULL) ;
         object->vCheckFeatures  (NULL, NULL) ;
         object->vCalculateShow  (time_c-RSS_Kernel::calc_time_step, time_c) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка сцены */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;                                                 /* END CIRCLE.1 - Цикл трассировки */

         object->trace_on=0 ;

/*-------------------------------------------------------------------*/

#undef    _COORD_MAX   
#undef     _PARS_MAX    
#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции STREAM                  */
/*								    */
/*      STREAM <Имя> <Файл телеметрии>                              */

  int  RSS_Module_Flyer::cStream(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта.\n"
                                      "Например: STREAM <Имя> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name, 1) ;                                 /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*---------------------------------------- Фиксация файла телеметрии */

  if(pars[1]==NULL) {
                       SEND_ERROR("Не задано имя файла телеметрии.\n"
                                  "Например: STREAM <Имя> <Файл телеметрии>") ;
                                     return(-1) ;
                    }

      strncpy(object->stream_path, pars[1], sizeof(object->stream_path)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа FLYER по имени                */

  RSS_Object_Flyer *RSS_Module_Flyer::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Flyer")) {

           SEND_ERROR("Объект не является объектом типа FLYER") ;
                            return(NULL) ;
                                           }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_Flyer *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*								     */
/*              Считывание файла описания программы                  */
/*								     */
/*  PROGRAM <Имя программы>                                          */
/*  OBJECT <Допустимый тип объекта>                                  */
/*  WAIT <Спецификатор:X,Y,Z,A,E,R,G,V>                              */
/*  BATTLE <Инструкция для Battle-задачи>                            */
/*								     */
/*  Спецификация закона движения:				     */
/*    <Спецификатор-1> <Спецификатор-2> ... <Спецификатор-N>         */
/*      T=<значение>      - метка времени    			     */
/*     DT=<приращение>    - изменение времени от предыдущего         */
/*   координаты объекта:                                             */
/*      X=<значение>                                                 */
/*      Y=<значение>                                                 */
/*      Z=<значение>                                                 */
/*   ориентация объекта (азимут, возвышение, крен)                   */
/*      A=<значение>                                                 */
/*      E=<значение>                                                 */
/*      R=<значение>                                                 */
/*   скорость изменения ориентация объекта (азимут, возвышение, крен)*/
/*     DA=<скорость>[:<до значения>]                                 */
/*     DE=<скорость>[:<до значения>]                                 */
/*     DR=<скорость>[:<до значения>]                                 */
/*   перегрузка маневра                                              */
/*      G=<значение>                                                 */
/*   скорость изменения перегрузки маневра                           */
/*     DG=<скорость>[:<до значения>]                                 */
/*   скорость                                                        */
/*      V=<значение>                                                 */
/*   ускорение                                                       */
/*     DV=<ускорение>[:<до значения>]                                */
/*								     */
/*  В качестве значения указывается либо число, либо имя объекта     */

  int  RSS_Module_Flyer::iReadProgram(RSS_Object_Flyer *object, char *path)
{
                    FILE *file ;
 RSS_Object_FlyerProgram *program ;
  RSS_Object_FlyerPFrame  frame ;
                    char  text[1024] ;
                    char  error[1024] ;
                    char *words[30] ;
                    char *name ;
                    char *data ;
                  double  value ;
                  double  target ;
                     int  target_flag ;
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

/*---------------------------------- Обработка спецификатора PROGRAM */

#define  _KEY_WORD  "PROGRAM "
      if(!memicmp(text, _KEY_WORD, strlen(_KEY_WORD))) {

               end=text+strlen(_KEY_WORD) ;
            if(*end==0) {
                          sprintf(error, "Строка %d - оператор PROGRAM не содержит имя программы", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }  

               program=NULL ;

        do {

         for(i=0 ; i<_PROGRAMS_MAX ; i++)
           if(object->programs[i]!=NULL)
            if(!stricmp(object->programs[i]->name, end)) {
                                                            program=object->programs[i] ;
                                                               break ;
                                                         }

            if(program!=NULL)  break ;

         for(i=0 ; i<_PROGRAMS_MAX ; i++)
            if(object->programs[i]==NULL) {
                                             program    =new RSS_Object_FlyerProgram ;
                                     object->programs[i]=program ;        
                                             break ;
                                          }
           } while(0) ;  

            if(program==NULL) {
                          sprintf(error, "Строка %d - количество программ для объекта превышает допустимый предел - %d", row, _PROGRAMS_MAX) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                              }

               strncpy(program->name, end, sizeof(program->name)-1) ;
                       program->frames_cnt=0 ;

                                       continue ;
                                                       }
#undef   _KEY_WORD

/*----------------------------------- Обработка спецификатора OBJECT */

#define  _KEY_WORD  "OBJECT "
      if(!memicmp(text, _KEY_WORD, strlen(_KEY_WORD))) {

               end=text+strlen(_KEY_WORD) ;
            if(*end==0) {
                          sprintf(error, "Строка %d - оператор OBJECT не содержит тип объекта", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }  

            if(stricmp(end, "Flyer")) {
                          sprintf(error, "Строка %d - программа не предназначена для объектов данного типа", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                                      }

                                       continue ;
                                                       }
#undef   _KEY_WORD

/*----------------------------------- Обработка спецификатора BATTLE */

#define  _KEY_WORD  "BATTLE "
      if(!memicmp(text, _KEY_WORD, strlen(_KEY_WORD))) {
/*- - - - - - - - - - - - - - - - - - - - - - - - Формирование кадра */
             end=text+strlen(_KEY_WORD) ;
         if(*end==0) {
                          sprintf(error, "Строка %d - оператор BATTLE не содержит передаваемую команду", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                     }  

            memset(&frame, 0, sizeof(frame)) ;
                    frame.battle_flag=1 ;
           strncpy( frame.used, end, sizeof(frame.used)-1) ;
/*- - - - - - - - - - - - - - - - - - - - -  Расшифровка подстановок */
//          end=strchr(frame.used, '*') ;
//       if(end!=NULL) {
//                         memmove(end+1, end+strlen(object->Name), strlen(end+1)+1) ;
//                          memcpy(end, object->Name, strlen(object->Name)) ;
//                     }
/*- - - - - - - - - - - - - - - - - - - - Сохранение кадра программы */
         if(program->frames_cnt>=_PFRAMES_MAX) {
               sprintf(error, "Строка %d - количество кадров программы превышает допустимый предел - %d", row, _PFRAMES_MAX) ;
            SEND_ERROR(error) ;
                  return(-1) ;
                                               }

            memcpy(&program->frames[program->frames_cnt], &frame, sizeof(frame)) ;
                                    program->frames_cnt++ ;

                                         continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                       }
#undef   _KEY_WORD

/*------------------------------------------- Разбор строки на слова */

            if(program==NULL) {
                                sprintf(error, "Строка %d - программа должна начинаться с оператора PROGRAM", row) ;
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

          strcpy( frame.used, ";") ;

       for(i=0 ; i<30 ; i++) {
                                 if(words[i]==NULL)  break ;

        if(stricmp(words[i], "WAIT")) {

             end=strchr(words[i], '=') ;
          if(end==NULL) {
                             sprintf(error, "Строка %d - в спецификаторе %d отсутствует символ разделитель '='", row, i+1) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }
            *end=0 ;
                                      }

              name=words[i] ;
              data=end+1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Метка времени */
          if(!stricmp(name, "WAIT")) {
                                         frame.wait_flag=1 ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Метка времени */
          else
          if(!stricmp(name, "T" )||
             !stricmp(name, "DT")  ) {

            if(!stricmp(name, "DT"))  frame.dt_flag=1 ;

               frame.t=strtod(data, &end) ;
            if(*end!=0) {
                             sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - Координаты объекта */
          else
          if(!stricmp(name, "X") ||
             !stricmp(name, "Y") ||
             !stricmp(name, "Z")   ) {

               value=strtod(data, &end) ;
            if(*end!=0) {
                             sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }

            if(!stricmp(name, "X"))  frame.x=value ;
            if(!stricmp(name, "Y"))  frame.y=value ;
            if(!stricmp(name, "Z"))  frame.z=value ;

                                 strcat(frame.used, name) ;
                                 strcat(frame.used, ";" ) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - Ориентация объекта */
          else
          if(!stricmp(name, "A") ||
             !stricmp(name, "E") ||
             !stricmp(name, "R")   ) {

               value=strtod(data, &end) ;
            if(*end!=0) {
                             sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }

            if(!stricmp(name, "A"))  frame.a=value ;
            if(!stricmp(name, "E"))  frame.e=value ;
            if(!stricmp(name, "R"))  frame.r=value ;

                                 strcat(frame.used, name) ;
                                 strcat(frame.used, ";" ) ;
                                     }
/*- - - - - - - - - - - - - -  Скорость изменения ориентации объекта */
          else
          if(!stricmp(name, "DA") ||
             !stricmp(name, "DE") ||
             !stricmp(name, "DR")   ) {

            if(frame.wait_flag) {
                          sprintf(error, "Строка %d - в операторе WAIT D-спецификаторы недопустимы", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                                }

                             target_flag= 0 ;
                              value     =strtod(data, &end) ;
            if(*end==':') {
                             target     =strtod(end+1, &end) ;
                             target_flag= 1  ;
                          }

            if(*end!=0) {
                          sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }

            if(!stricmp(name, "DA"))  frame.d_a=value ;
            if(!stricmp(name, "DE"))  frame.d_e=value ;
            if(!stricmp(name, "DR"))  frame.d_r=value ;

                               strcat(frame.used, name) ;
                               strcat(frame.used, ";" ) ;

            if(target_flag==0)  continue ; 

                        name[0]='T' ;

            if(!stricmp(name, "TA"))  frame.t_a=target ;
            if(!stricmp(name, "TE"))  frame.t_e=target ;
            if(!stricmp(name, "TR"))  frame.t_r=target ;

                               strcat(frame.used, name) ;
                               strcat(frame.used, ";" ) ;

                                      }
/*- - - - - - - - - - - - - - - - - - - - - - -  Перегрузка маневра */
          else
          if(!stricmp(name, "G")) {

               value=strtod(data, &end) ;
            if(*end!=0) {
                             sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }

                                      frame.g=value ;

                                 strcat(frame.used, name) ;
                                 strcat(frame.used, ";" ) ;
                                     }
/*- - - - - - - - - - - - - -  Скорость изменения перегрузка маневра */
          else
          if(!stricmp(name, "DG")) {

            if(frame.wait_flag) {
                          sprintf(error, "Строка %d - в операторе WAIT D-спецификаторы недопустимы", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                                }

                             target_flag= 0 ;
                              value     =strtod(data, &end) ;
            if(*end==':') {
                             target     =strtod(end+1, &end) ;
                             target_flag= 1  ;
                          }

            if(*end!=0) {
                          sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }

                                      frame.d_g=value ;
                               strcat(frame.used, name) ;
                               strcat(frame.used, ";" ) ;

            if(target_flag==0)  continue ;

                                      frame.t_g=target ;
                               strcat(frame.used, "TG;") ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Скорость */
          else
          if(!stricmp(name, "V")) {

               value=strtod(data, &end) ;
            if(*end!=0) {
                             sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;
                        }

                                      frame.v=value ;

                                 strcat(frame.used, name) ;
                                 strcat(frame.used, ";" ) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Ускорение */
          else
          if(!stricmp(name, "DV")) {

            if(frame.wait_flag) {
                          sprintf(error, "Строка %d - в операторе WAIT D-спецификаторы недопустимы", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                                }

                             target_flag= 0 ;
                              value     =strtod(data, &end) ;
            if(*end==':') {
                             target     =strtod(end+1, &end) ;
                             target_flag= 1  ;
                          }

            if(*end!=0) {
                          sprintf(error, "Строка %d - в спецификаторе %s задано некорректное значение", row, name) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }

                                      frame.d_v=value ;
                               strcat(frame.used, name) ;
                               strcat(frame.used, ";" ) ;

            if(target_flag==0)  continue ;

                                      frame.t_v=target ;
                               strcat(frame.used, "TV;") ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - - Неизвестный спецификатор */
          else                    {

                             sprintf(error, "Строка %d - неизвестный спецификатор %s", row, name) ;
                          SEND_ERROR(error) ;
                                return(-1) ;

                                  } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                             }
/*--------------------------------------- Сохранение кадра программы */

       if(program->frames_cnt>=_PFRAMES_MAX) {
               sprintf(error, "Строка %d - количество кадров программы превышает допустимый предел - %d", row, _PFRAMES_MAX) ;
            SEND_ERROR(error) ;
                  return(-1) ;
                                             }

       memcpy(&program->frames[program->frames_cnt], &frame, sizeof(frame)) ;
                               program->frames_cnt++ ;

/*-------------------------------------------------------------------*/
            }                                                       /* CONTINUE.1 */
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Добавление компонента к объекту                   */

  class RSS_Unit *RSS_Module_Flyer::AddUnit(RSS_Object_Flyer *object, char *unit_name, char *unit_type, char *error)

{
   RSS_Kernel *unit_module ;
     RSS_Unit *unit ;
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

        unit=(RSS_Unit *)unit_module->vCreateObject(NULL) ;
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
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "ЛЕТУН"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_Flyer::RSS_Object_Flyer(void)

{
   strcpy(Type, "Flyer") ;

    Context        =new RSS_Transit_Flyer ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     battle_state= 0 ;

      v_abs     = 0. ;
      g_ctrl    = 0. ;

      r_ctrl    = 0. ;
      m_ctrl    =NULL ;

    memset(events,    0, sizeof(events)) ;
    memset(programs,  0, sizeof(programs)) ;
    memset(battle_cb, 0, sizeof(battle_cb)) ;

    memset(stream_path, 0, sizeof(stream_path)) ;

      trace_on    =  0 ;
      trace_time  =  0 ;

      mTrace_flag =  1 ;
      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=  0 ;  
      mTrace_dlist=  0 ;  
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Flyer::~RSS_Object_Flyer(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Object_Flyer::vFree(void)

{
  int  i ;


   for(i=0 ; i<_PROGRAMS_MAX ; i++)  
     if(programs[i]!=NULL) {
                              delete programs[i] ;
                                     programs[i]=NULL ;
                           }

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
/*                        Копировать объект		            */

    class RSS_Object *RSS_Object_Flyer::vCopy(char *name)

{
           RSS_Model_data  create_data ;
         RSS_Object_Flyer *object ;
  RSS_Object_FlyerProgram *program ;
                 RSS_Unit *unit ;
          RSS_Feature_Hit *hit_1 ;
          RSS_Feature_Hit *hit_2 ;
                      int  i ;

/*------------------------------------- Копирование базового объекта */

                    memset(&create_data, 0, sizeof(create_data)) ;

     if(name!=NULL)  strcpy( create_data.name, name) ;
                    strcpy( create_data.path, this->model_path) ;

  for(i=0 ; i<this->Parameters_cnt ; i++) {
         sprintf(create_data.pars[i].text,  "PAR%d", i) ;
         sprintf(create_data.pars[i].value, "%lf", this->Parameters[i].value) ;
                                          }

                 create_data.pars[i].text [0]=0 ;
                 create_data.pars[i].value[0]=0 ;

        object=(RSS_Object_Flyer *)this->Module->vCreateObject(&create_data) ;
     if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;
            strcpy(object->target, this->target) ;

                   object->v_abs            =this->v_abs ;          /* Нормальная скорость */
                   object->g_ctrl           =this->g_ctrl ;         /* Нормальная траекторная перегрузка */

     if(RSS_Kernel::battle)  object->battle_state=_SPAWN_STATE ;

  for(i=0 ; i<_PROGRAMS_MAX ; i++)                                  /* Программы управления */
    if(this->programs[i]!=NULL) {

              program =new RSS_Object_FlyerProgram ;
       strcpy(program->name,      this->programs[i]->name) ; 
       memcpy(program->frames,    this->programs[i]->frames, sizeof(this->programs[i]->frames)) ; 
              program->frames_cnt=this->programs[i]->frames_cnt ;

              object->programs[i]=program ;        
                                }

       memcpy(object->events, this->events, sizeof(this->events)) ; /* Программы реакции на события */

/*---------------------------------------------- Копирование свойств */

    for(i=0 ; i<this->Features_cnt ; i++)
      if(!stricmp(this->Features[i]->Type, "Hit"))
             hit_1=(RSS_Feature_Hit *)this->Features[i] ;

    for(i=0 ; i<object->Features_cnt ; i++)
      if(!stricmp(object->Features[i]->Type, "Hit"))
             hit_2=(RSS_Feature_Hit *)object->Features[i] ;

           hit_2->hit_range =hit_1->hit_range ;
           hit_2->any_target=hit_1->any_target ;
           hit_2->any_weapon=hit_1->any_weapon ;

/*---------------------------------------------- Копирование модулей */

    for(i=0 ; i<Units.List_cnt ; i++) {
                 
        unit=((RSS_Unit *)(Units.List[i].object))->vCopy(object) ;
     if(unit==NULL)   return(NULL) ;

                  object->Units.Add(unit, "") ;

                                      }
/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*								    */
/*                    Сохранить состояние объекта                   */
/*                    Восстановить состояние объекта                */

    void  RSS_Object_Flyer::vPush(void)

{
     state_stack=state ;

     v_abs_stack     =v_abs ;
     g_ctrl_stack    =g_ctrl ;
}


    void  RSS_Object_Flyer::vPop(void)

{
     state =state_stack ;
     v_abs =v_abs_stack ;
     g_ctrl=g_ctrl_stack ;

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

    void  RSS_Object_Flyer::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT FLYER\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n",       this->Name      ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->state.x   ) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->state.y   ) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->state.z   ) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->state.azim) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->state.elev) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->state.roll) ;  *text+=field ;
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
/*                        Специальные действия                      */

     int  RSS_Object_Flyer::vSpecial(char *oper, void *data)
{
   int  i ;

/*----------------------------------------- Трансляция на компоненты */

    for(i=0 ; i<this->Units.List_cnt ; i++)
      this->Units.List[i].object->vSpecial(oper, data) ;
    
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_Flyer::vCalculateStart(double  t)
{
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;  
    Matrix2d  Velo_Matrix ;  
        FILE *file ;
        char  text[1024] ;
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

      this->state.x   =this->o_owner->state_0.x ;
      this->state.y   =this->o_owner->state_0.y ;
      this->state.z   =this->o_owner->state_0.z ;

      this->state.azim=this->o_owner->state_0.azim ;
      this->state.elev=this->o_owner->state_0.elev ;
      this->state.roll=this->o_owner->state_0.roll ;
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
        Sum_Matrix.Load3d_azim(-this->state.azim) ;
       Oper_Matrix.Load3d_elev(-this->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         this->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
         this->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
         this->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*------------------------------------------------------- Подготовка */

             this->program=NULL ;

             this->iSaveTracePoint("CLEAR") ;

      memset(this->direct_select, 0, sizeof(this->direct_select)) ;

    for(i=0 ; i<Units.List_cnt ; i++)                               /* Инициализация подчиненных объектов */
                  Units.List[i].object->vCalculateStart(t) ;

/*--------------------------------------------- Обработка телеметрии */

   if(this->stream_path[0]!=0) {

        file=fopen(this->stream_path, "w") ;
     if(file==NULL) {
                         sprintf(text, "%s - Stream file open error %d : %s", this->Name, errno, this->stream_path) ;
                      SEND_ERROR(text) ;
                    }
     else           {
                         sprintf(text, "%.5lf;"
                                       "%.5lf;%.5lf;%.5lf;"
                                       "%.5lf;%.5lf;%.5lf;"
                                       "%.5lf;"
                                       "\n", 
                                         t,
                                         this->state.x,    this->state.y,    this->state.z,
                                         this->state.azim, this->state.elev, this->state.roll,
                                         this->v_abs
                                ) ;
                          fwrite(text, 1, strlen(text), file) ;
                          fclose(file) ;
                    }         

                               }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_Flyer::vCalculate(double t1, double t2, char *callback, int  callback_size)
{
  Matrix2d  Sum_Matrix ;
  Matrix2d  Oper_Matrix ;
  Matrix2d  Vect_Matrix ;  
  Matrix2d  Gold_Matrix ;  
  Matrix2d  Gnew_Matrix ;  
    double  r ;                  /* Радиус маневра */
    double  a ;                  /* Ометываемый угол маневра */
    double  s1, s2 ;
    double  x1, y1, z1 ;         /* Точка синуса ометываемого угла маневра */
    double  n1_azim, n1_elev ;
    double  n2_azim, n2_elev ;
    double  x2, y2, z2 ;         /* Точка синуса ометываемого угла маневра */
       int  i ;

/*----------------------------------- Отработка подчиненных объектов */

    for(i=0 ; i<Units.List_cnt ; i++)
      Units.List[i].object->vCalculate(t1, t2, callback, callback_size) ;

/*------------------------------------- Обработка целевого состояния */

  if(program==NULL)                                                 /* Если не действует событийная программы управления... */
   if(this->direct_select[0]!=0) {
/*- - - - - - - - - - - - - - - - - - - -  Расчёт заданных положений */
          strupr(this->direct_select) ;

       if(strchr(this->direct_select, 'X')!=NULL)  state.x   =this->direct_target.x ;
       if(strchr(this->direct_select, 'Y')!=NULL)  state.y   =this->direct_target.y ;
       if(strchr(this->direct_select, 'Z')!=NULL)  state.z   =this->direct_target.z ;
       if(strchr(this->direct_select, 'A')!=NULL)  state.azim=this->direct_target.azim ;
       if(strchr(this->direct_select, 'E')!=NULL)  state.elev=this->direct_target.elev ;
       if(strchr(this->direct_select, 'R')!=NULL)  state.roll=this->direct_target.roll ;
/*- - - - - - - - - - - - - - - - - - - - - Расчёт проекций скорости */
                Vect_Matrix.LoadZero   (3, 1) ;
                Vect_Matrix.SetCell    (2, 0, v_abs) ;

                 Sum_Matrix.Load3d_azim(-state.azim) ;
                Oper_Matrix.Load3d_elev(-state.elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Vect_Matrix.LoadMul    (&Sum_Matrix, &Vect_Matrix) ;

                 state.x_velocity=Vect_Matrix.GetCell(0, 0) ;
                 state.y_velocity=Vect_Matrix.GetCell(1, 0) ;
                 state.z_velocity=Vect_Matrix.GetCell(2, 0) ;
/*- - - - - - - - - - - - - - - - - - - - Расчёт свободных положений */
       if(strchr(this->direct_select, 'X')==NULL)  state.x+=state.x_velocity*(t2-t1) ;
       if(strchr(this->direct_select, 'Y')==NULL)  state.y+=state.y_velocity*(t2-t1) ;
       if(strchr(this->direct_select, 'Z')==NULL)  state.z+=state.z_velocity*(t2-t1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          memset(this->direct_select, 0, sizeof(this->direct_select)) ;

                                        return(0) ;
                                 }
/*---------------------------------------------- Отработка программы */

                          trace_time=t2 ;

   if(program!=NULL)  iExecuteProgram(t1, t2) ;

   if( callback !=NULL &&
      *battle_cb!=  0    ) {
                      strncpy(callback, battle_cb, callback_size) ;
                                       *battle_cb=0 ;
                           }
/*---------------------------------------------- Постоянная скорость */

   if(this->g_ctrl==0) {
/*- - - - - - - - - - - - - - - - - - - -  Расчёт по старому вектору */
                          state.x+=state.x_velocity*(t2-t1) ;
                          state.y+=state.y_velocity*(t2-t1) ;
                          state.z+=state.z_velocity*(t2-t1) ;
/*- - - - - - - - - - - - - Сброс контекста управления по перегрузке */
     if(m_ctrl!=NULL) {
                         delete m_ctrl ;
                                m_ctrl=NULL ;
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                       }
/*-------------------------------------------- Постоянная перегрузка */

   else                {
/*- - - - - - - - - - - - - - - - - - - - - - - -  Параметры маневра */
                           r      = v_abs*v_abs/g_ctrl ;
                           a      =(360.*v_abs*(t2-t1))/(2.*_PI*r) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Матрица преобразования */
                 Sum_Matrix.Load3d_azim(-state.azim) ;
                Oper_Matrix.Load3d_elev(-state.elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Oper_Matrix.Load3d_roll(-state.roll) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;

    if(r_ctrl!=state.roll ||
       m_ctrl==  NULL       ) {
                if(m_ctrl==NULL) m_ctrl=new Matrix2d ;
 
                                 m_ctrl->Copy(&Sum_Matrix) ;
                                 r_ctrl=state.roll ;
                                 a_ctrl=   0. ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - -  Расчет конечной точки */
                         x1=0 ;
                         y1=r-r*cos(a*_GRD_TO_RAD) ;
                         z1=  r*sin(a*_GRD_TO_RAD) ;

                Vect_Matrix.LoadZero(3, 1) ;
                Vect_Matrix.SetCell (0, 0, x1) ;
                Vect_Matrix.SetCell (1, 0, y1) ;
                Vect_Matrix.SetCell (2, 0, z1) ;

                Vect_Matrix.LoadMul (&Sum_Matrix, &Vect_Matrix) ;

                    state.x+=Vect_Matrix.GetCell(0, 0) ;
                    state.y+=Vect_Matrix.GetCell(1, 0) ;
                    state.z+=Vect_Matrix.GetCell(2, 0) ;
/*- - - - - - - - - - - - - - - - - - - - - Расчет проекций скорости */
                     a_ctrl+=a ;
                         x1 =0. ;
                         y1 =this->v_abs*sin(a_ctrl*_GRD_TO_RAD) ;
                         z1 =this->v_abs*cos(a_ctrl*_GRD_TO_RAD) ;

                Vect_Matrix.LoadZero(3, 1) ;
                Vect_Matrix.SetCell (0, 0, x1) ;
                Vect_Matrix.SetCell (1, 0, y1) ;
                Vect_Matrix.SetCell (2, 0, z1) ;

                Vect_Matrix.LoadMul (m_ctrl, &Vect_Matrix) ;

                    state.x_velocity=Vect_Matrix.GetCell(0, 0) ;
                    state.y_velocity=Vect_Matrix.GetCell(1, 0) ;
                    state.z_velocity=Vect_Matrix.GetCell(2, 0) ;
/*- - - - - - - - - - - - - - - - - - - - Изменение углов ориентации */
                Gold_Matrix.LoadZero(3, 1) ;
                Gold_Matrix.SetCell (1, 0, 1.) ;
                Gold_Matrix.LoadMul (&Sum_Matrix, &Gold_Matrix) ;

                  n1_azim=atan2(state.x_velocity, state.z_velocity)*_RAD_TO_GRD ;
                  n1_elev=atan2(state.y_velocity, sqrt(state.x_velocity*state.x_velocity+state.z_velocity*state.z_velocity))*_RAD_TO_GRD ;
                  n2_azim=180.+n1_azim ;
                  n2_elev=180.-n1_elev ;

                 Sum_Matrix.Load3d_azim(-n1_azim) ;
                Oper_Matrix.Load3d_elev(-n1_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Oper_Matrix.Load3d_roll(-state.roll) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                
                Gnew_Matrix.LoadZero(3, 1) ;
                Gnew_Matrix.SetCell (1, 0, 1.) ;
                Gnew_Matrix.LoadMul (&Sum_Matrix, &Gnew_Matrix) ;

              x1=Gold_Matrix.GetCell(0, 0)-Gnew_Matrix.GetCell(0, 0) ;
              y1=Gold_Matrix.GetCell(1, 0)-Gnew_Matrix.GetCell(1, 0) ;
              z1=Gold_Matrix.GetCell(2, 0)-Gnew_Matrix.GetCell(2, 0) ;

              s1=sqrt(x1*x1+y1*y1+z1*z1) ;
           if(s1>0.) {  x1/=s1 ;  y1/=s1 ;  z1/=s1 ;  }

                 Sum_Matrix.Transpose(m_ctrl) ;
                Vect_Matrix.LoadZero (3, 1) ;
                Vect_Matrix.SetCell  (0, 0, x1) ;
                Vect_Matrix.SetCell  (1, 0, y1) ;
                Vect_Matrix.SetCell  (2, 0, z1) ;
                Vect_Matrix.LoadMul  (&Sum_Matrix, &Vect_Matrix) ;

             x2=Vect_Matrix.GetCell  (0, 0) ;
             y2=Vect_Matrix.GetCell  (1, 0) ;
             z2=Vect_Matrix.GetCell  (2, 0) ;

                 Sum_Matrix.Load3d_azim(-n2_azim) ;
                Oper_Matrix.Load3d_elev(-n2_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Oper_Matrix.Load3d_roll(-state.roll) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                
                Gnew_Matrix.LoadZero(3, 1) ;
                Gnew_Matrix.SetCell (1, 0, 1.) ;
                Gnew_Matrix.LoadMul (&Sum_Matrix, &Gnew_Matrix) ;

              x2=Gold_Matrix.GetCell(0, 0)-Gnew_Matrix.GetCell(0, 0) ;
              y2=Gold_Matrix.GetCell(1, 0)-Gnew_Matrix.GetCell(1, 0) ;
              z2=Gold_Matrix.GetCell(2, 0)-Gnew_Matrix.GetCell(2, 0) ;

              s2=sqrt(x2*x2+y2*y2+z2*z2) ;
//           if(s2>0.) {  x2/=s2 ;  y2/=s2 ;  z2/=s2 ;  }

                 Sum_Matrix.Transpose(m_ctrl) ;
                Vect_Matrix.LoadZero (3, 1) ;
                Vect_Matrix.SetCell  (0, 0, x2) ;
                Vect_Matrix.SetCell  (1, 0, y2) ;
                Vect_Matrix.SetCell  (2, 0, z2) ;
                Vect_Matrix.LoadMul  (&Sum_Matrix, &Vect_Matrix) ;

             x2=Vect_Matrix.GetCell  (0, 0) ;

            if(     s1 > 1.     ) {
                                      state.azim=n2_azim ;
                                      state.elev=n2_elev ;
                                  }
            else
            if(     s2 > 1.     ) {
                                      state.azim=n1_azim ;
                                      state.elev=n1_elev ;
                                  }
            else
            if(fabs(x1)<fabs(x2)) {
                                      state.azim=n1_azim ;
                                      state.elev=n1_elev ;
                                  }
            else                  {
                                      state.azim=n2_azim ;
                                      state.elev=n2_elev ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                       }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_Flyer::vCalculateShow(double t1, double t2)
{
  FILE *file ;
  char  text[1024] ;
   int  i ;

/*-------------------------------------------- Обработка отображения */

    for(i=0 ; i<Units.List_cnt ; i++)                               /* Отработка подчененных объектов */ 
                  Units.List[i].object->vCalculateShow(t1, t2) ;

         this->iSaveTracePoint("ADD") ;                             /* Сохранение точки траектории */  

         this->iShowTrace_() ;                                      /* Отображение траектории */

   for(i=0 ; i<this->Features_cnt ; i++) {                          /* Отображение объекта */
     this->Features[i]->vBodyBasePoint(NULL, this->state.x, 
                                             this->state.y, 
                                             this->state.z ) ;
     this->Features[i]->vBodyAngles   (NULL, this->state.azim, 
                                             this->state.elev, 
                                             this->state.roll ) ;
     this->Features[i]->vShow         (NULL) ;
                                         }
/*--------------------------------------------- Обработка телеметрии */

   if(this->stream_path[0]!=0) {

        file=fopen(this->stream_path, "a+") ;
     if(file==NULL) {
                         sprintf(text, "%s - Stream file open error %d : %s", this->Name, errno, this->stream_path) ;
                      SEND_ERROR(text) ;
                    }
     else           {
                         sprintf(text, "%.5lf;"
                                       "%.5lf;%.5lf;%.5lf;"
                                       "%.5lf;%.5lf;%.5lf;"
                                       "%.5lf;"
                                       "\n", 
                                         t2,
                                         this->state.x,    this->state.y,    this->state.z,
                                         this->state.azim, this->state.elev, this->state.roll,
                                         this->v_abs
                                ) ;
                          fwrite(text, 1, strlen(text), file) ;
                          fclose(file) ;
                    }         

                               }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                        Обработка событий                         */

     int  RSS_Object_Flyer::vEvent(char *event_name, double  t, char *callback, int cb_size)
{
  int n ;
  int i ;

/*-------------------------------------------- Идентификация события */

        for(n=0 ; n<_EVENTS_MAX ; n++)                              /* Проверяем, нет ли такого события */
          if(!stricmp(events[n].name, event_name))  break ; 

          if(n>=_EVENTS_MAX)   return(-1) ;

/*------------------------------------------ Идентификация программы */

        for(i=0 ; i<_PROGRAMS_MAX ; i++)
          if(programs[i]!=NULL)
           if(!stricmp(programs[i]->name, events[n].program))   break ;

          if(i>=_PROGRAMS_MAX)  return(-1) ;

/*----------------------------------------------- Загрузка программы */

               program=programs[i] ;
       memset(&p_controls, 0, sizeof(p_controls)) ;
       strcpy( p_controls.used, ";") ;
               p_frame=0 ;
               p_start=t ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*               Отработка программного управления                  */
/*								    */
/*  ВНИМАНИЕ! Параметры контекста управления должны храниться в     */
/*            экземпляре класса, а не как static                    */

     int  RSS_Object_Flyer::iExecuteProgram(double t1, double t2)
{
       Matrix2d  Sum_Matrix ;
       Matrix2d  Oper_Matrix ;  
       Matrix2d  Velo_Matrix ;  
            int  v_flag ;    /* Флаг перерасчета проекций скорости */
           char  used[1024] ;
           char  frame_used[1024] ;
         double  x_new, y_new, z_new, a_new, e_new, r_new, g_new, v_new ;
         double  value ;
            int  status ;      
           char *end ;
            int  i ;

#define  FRAME program->frames[i]

/*----------------------------------- Контроль завершения исполнения */

       if(p_frame>=program->frames_cnt)  return(0) ;

/*----------------------- Пересчет абсолютного времени в программное */

                 t1-=p_start ;
                 t2-=p_start ;

/*---------------------------------------------------- Инициализация */

       if(p_frame==0) {
                          t_prv= t1 ;
                          x_prv=this->state.x ;
                          y_prv=this->state.y ;
                          z_prv=this->state.z ;
                          a_prv=this->state.azim ;
                          e_prv=this->state.elev ;
                          r_prv=this->state.roll ;
                          v_prv=this->v_abs ;
                          g_prv=this->g_ctrl ;
                      }
/*--------------------------------------- Обработка кадров программы */

             v_flag=0 ;

   for(i=p_frame ; i<program->frames_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - - -  Расшифровка подстановок */
           memset(frame_used,   0,                     sizeof(used)  ) ;
          strncpy(frame_used, program->frames[i].used, sizeof(used)-1) ;

        end=strchr(frame_used, '*') ;
     if(end!=NULL) {
                       memmove(end+1, end+strlen(this->Name), strlen(end+1)+1) ;
                        memcpy(end, this->Name, strlen(this->Name)) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Оператор BATTLE */
     if(program->frames[i].battle_flag) {
             
             strcat(this->battle_cb, frame_used) ;
             strcat(this->battle_cb, ";") ;
                        p_frame++ ;
                         continue ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - -  Обработка метки времени */
     if(program->frames[i].wait_flag==0) {

      if(program->frames[i].dt_flag==0) {
             if(program->frames[i].t      >t2)  break ;
                                        }
      else                              {
             if(program->frames[i].t+t_prv>t2)  break ;
                                        }
                                         }

                     t_prv= t2 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Оператор WAIT */
     if(program->frames[i].wait_flag) {
                                         break ;
                                      }
/*- - - - - - - - - - - - - - - - - - - - - - -  Обработка координат */
     if(strstr(FRAME.used, ";X;"))    this->state.x=FRAME.x ;
     if(strstr(FRAME.used, ";Y;"))    this->state.y=FRAME.y ;
     if(strstr(FRAME.used, ";Z;"))    this->state.z=FRAME.z ;

     if(strstr(FRAME.used, ";A;")) {  this->state.azim=FRAME.a ;  v_flag=1 ;  }
     if(strstr(FRAME.used, ";E;")) {  this->state.elev=FRAME.e ;  v_flag=1 ;  }
     if(strstr(FRAME.used, ";R;"))    this->state.roll=FRAME.r ;
/*- - - - - - - - - - - - - - - - - -  Обработка изменения координат */
     if(strstr(FRAME.used, ";DA;")) {
                        iReplaceText(this->p_controls.used, ";DA;", ";", 1) ;
                        iReplaceText(this->p_controls.used, ";TA;", ";", 1) ;

                                     this->p_controls.d_a=FRAME.d_a ;
         if(FRAME.d_a!=0.)    strcat(this->p_controls.used, "DA;") ;
                                    }
     if(strstr(FRAME.used, ";DE;")) {
                        iReplaceText(this->p_controls.used, ";DE;", ";", 1) ;
                        iReplaceText(this->p_controls.used, ";TE;", ";", 1) ;

                                     this->p_controls.d_e=FRAME.d_e ;
         if(FRAME.d_e!=0.)    strcat(this->p_controls.used, "DE;") ;
                                    }
     if(strstr(FRAME.used, ";DR;")) {
                        iReplaceText(this->p_controls.used, ";DR;", ";", 1) ;
                        iReplaceText(this->p_controls.used, ";TR;", ";", 1) ;

                                     this->p_controls.d_r=FRAME.d_r ;
         if(FRAME.d_r!=0.)    strcat(this->p_controls.used, "DR;") ;
                                    }

     if(strstr(FRAME.used, ";TA;")) {
                                       this->p_controls.t_a=FRAME.t_a ;
                                strcat(this->p_controls.used, "TA;") ;
                                    }
     if(strstr(FRAME.used, ";TE;")) {
                                       this->p_controls.t_e=FRAME.t_e ;
                                strcat(this->p_controls.used, "TE;") ;
                                    }
     if(strstr(FRAME.used, ";TR;")) {
                                       this->p_controls.t_r=FRAME.t_r ;
                                strcat(this->p_controls.used, "TR;") ;
                                    }
/*- - - - - - - - - - - - - - - - - - - Обработка перегрузки маневра */
     if(strstr(FRAME.used, ";G;"))    this->g_ctrl=FRAME.g ;

     if(strstr(FRAME.used, ";DG;")) {
                        iReplaceText(this->p_controls.used, ";DG;", ";", 1) ;
                        iReplaceText(this->p_controls.used, ";TG;", ";", 1) ;

                                     this->p_controls.d_g=FRAME.d_g ;
         if(FRAME.d_g!=0.)    strcat(this->p_controls.used, "DG;") ;
                                    }

     if(strstr(FRAME.used, ";TG;")) {
                                       this->p_controls.t_g=FRAME.t_g ;
                                strcat(this->p_controls.used, "TG;") ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - Обработка скорости */
     if(strstr(FRAME.used, ";V;")) {  this->v_abs=FRAME.v ;  v_flag=1 ;  }

     if(strstr(FRAME.used, ";DV;")) {
                        iReplaceText(this->p_controls.used, ";DV;", ";", 1) ;
                        iReplaceText(this->p_controls.used, ";TV;", ";", 1) ;

                                     this->p_controls.d_v=FRAME.d_v ;
           if(FRAME.d_v!=0.)  strcat(this->p_controls.used, "DV;") ;
                                    }

     if(strstr(FRAME.used, ";TV;")) {
                                       this->p_controls.t_v=FRAME.t_v ;
                                strcat(this->p_controls.used, "TV;") ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - Обработка кадров программы */
                 p_frame++ ;
                                                }
/*------------------------------------ Перерасчет изменяемых величин */

         memset(used,   0,                   sizeof(used)  ) ;
        strncpy(used, this->p_controls.used, sizeof(used)-1) ;

     if(strstr(this->p_controls.used, ";DA;"))  a_new=this->state.azim+this->p_controls.d_a*(t2-t1) ;
     if(strstr(this->p_controls.used, ";DE;"))  e_new=this->state.elev+this->p_controls.d_e*(t2-t1) ;
     if(strstr(this->p_controls.used, ";DR;"))  r_new=this->state.roll+this->p_controls.d_r*(t2-t1) ;
     if(strstr(this->p_controls.used, ";DG;"))  g_new=this->g_ctrl    +this->p_controls.d_g*(t2-t1) ;
     if(strstr(this->p_controls.used, ";DV;"))  v_new=this->v_abs     +this->p_controls.d_v*(t2-t1) ;

/*----------------------------- Контроль граничных условий изменений */

     if(strstr(this->p_controls.used, ";TA;")) {

       if(a_new>this->state.azim)  status=iAngleInCheck(this->p_controls.t_a, this->state.azim, a_new) ;
       else                        status=iAngleInCheck(this->p_controls.t_a, a_new, this->state.azim) ;

       if(!status) {
                            a_new=this->p_controls.t_a ;
                     iReplaceText(this->p_controls.used, ";DA;", ";", 1) ;
                     iReplaceText(this->p_controls.used, ";TA;", ";", 1) ;
                   }
                                               }

     if(strstr(this->p_controls.used, ";TE;")) {

       if(e_new>this->state.elev)  status=iAngleInCheck(this->p_controls.t_e, this->state.elev, e_new) ;
       else                        status=iAngleInCheck(this->p_controls.t_e, e_new, this->state.elev) ;

       if(!status) {
                            e_new=this->p_controls.t_e ;
                     iReplaceText(this->p_controls.used, ";DE;", ";", 1) ;
                     iReplaceText(this->p_controls.used, ";TE;", ";", 1) ;
                   }
                                               }

     if(strstr(this->p_controls.used, ";TR;")) {

       if(r_new>this->state.roll)  status=iAngleInCheck(this->p_controls.t_r, this->state.roll, r_new) ;
       else                        status=iAngleInCheck(this->p_controls.t_r, r_new, this->state.roll) ;

       if(!status) {
                            e_new=this->p_controls.t_r ;
                     iReplaceText(this->p_controls.used, ";DR;", ";", 1) ;
                     iReplaceText(this->p_controls.used, ";TR;", ";", 1) ;
                   }
                                               }

     if(strstr(this->p_controls.used, ";TG;")) {

       if(g_new>this->g_ctrl)  status=iAngleInCheck(this->p_controls.t_g, this->g_ctrl, g_new) ;
       else                    status=iAngleInCheck(this->p_controls.t_g, g_new, this->g_ctrl) ;

       if(!status) {
                            g_new=this->p_controls.t_g ;
                     iReplaceText(this->p_controls.used, ";DG;", ";", 1) ;
                     iReplaceText(this->p_controls.used, ";TG;", ";", 1) ;
                   }
                                               }

     if(strstr(this->p_controls.used, ";TV;")) {

       if(v_new>this->v_abs)  status=iAngleInCheck(this->p_controls.t_v, this->v_abs, v_new) ;
       else                   status=iAngleInCheck(this->p_controls.t_v, v_new, this->v_abs) ;

       if(!status) {
                            v_new=this->p_controls.t_v ;
                     iReplaceText(this->p_controls.used, ";DV;", ";", 1) ;
                     iReplaceText(this->p_controls.used, ";TV;", ";", 1) ;
                   }
                                               }
/*------------------------------------ Присвоение изменяемых величин */

     if(strstr(used, ";DA;")) {  this->state.azim=a_new ;  v_flag=1 ;  }
     if(strstr(used, ";DE;")) {  this->state.elev=e_new ;  v_flag=1 ;  }
     if(strstr(used, ";DR;"))    this->state.roll=r_new ;
     if(strstr(used, ";DG;"))    this->g_ctrl    =g_new ;
     if(strstr(used, ";DV;")) {  this->v_abs     =v_new ;  v_flag=1 ;  }

/*------------------------------------- Перерасчет проекций скорости */

  if(v_flag) {
                   Velo_Matrix.LoadZero   (3, 1) ;
                   Velo_Matrix.SetCell    (2, 0, this->v_abs) ;
                    Sum_Matrix.Load3d_azim(-this->state.azim) ;
                   Oper_Matrix.Load3d_elev(-this->state.elev) ;
                    Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                   Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

                state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
                state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
                state.z_velocity=Velo_Matrix.GetCell(2, 0) ;
             }
/*-------------------------------------- Анализ прекращения ожидания */

#define  WAIT  program->frames[p_frame]

  if(WAIT.wait_flag) {
/*- - - - - - - - - - - - - - - - - - - - -  Достижение X-координаты */
     if(strstr(WAIT.used, ";X;")) {
                                       x_new =this->state.x ;
                                       value =WAIT.x ;

       if( (value>=x_new && value<=x_prv) ||
           (value>=x_prv && value<=x_new)   )  status=0 ;
       else                                    status=1 ;

       if(!status)   p_frame++ ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - -  Достижение Y-координаты */
     if(strstr(WAIT.used, ";Y;")) {
                                       y_new =this->state.y ;
                                       value =WAIT.y ;

       if( (value>=y_new && value<=y_prv) ||
           (value>=y_prv && value<=y_new)   )  status=0 ;
       else                                    status=1 ;

       if(!status)   p_frame++ ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - -  Достижение Z-координаты */
     if(strstr(WAIT.used, ";Z;")) {
                                       z_new =this->state.z ;
                                       value =WAIT.z ;

       if( (value>=z_new && value<=z_prv) ||
           (value>=z_prv && value<=z_new)   )  status=0 ;
       else                                    status=1 ;

       if(!status)   p_frame++ ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - Достижение азимута */
     if(strstr(WAIT.used, ";A;")) {
                                       a_new =this->state.azim ;
                                       value =WAIT.a ;

                       while(a_new<0.) a_new+=360. ;
                       while(a_prv<0.) a_prv+=360. ;
                       while(value<0.) value+=360. ;

       if(fabs(a_new-a_prv) > 180.)  
        if(a_new>a_prv)  a_prv+=360. ;
        else             a_new+=360. ;

       if(a_new>a_prv)  status=iAngleInCheck(value, a_prv, a_new) ;
       else             status=iAngleInCheck(value, a_new, a_prv) ;

       if(!status)   p_frame++ ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - Достижение угла возвышения */
     if(strstr(WAIT.used, ";E;")) {
                                       e_new =this->state.elev ;
                                       value =WAIT.e ;

                       while(e_new<0.) e_new+=360. ;
                       while(e_prv<0.) e_prv+=360. ;
                       while(value<0.) value+=360. ;

       if(fabs(e_new-e_prv) > 180.)  
        if(e_new>e_prv)  e_prv+=360. ;
        else             e_new+=360. ;

       if(e_new>e_prv)  status=iAngleInCheck(value, e_prv, e_new) ;
       else             status=iAngleInCheck(value, e_new, e_prv) ;

       if(!status)   p_frame++ ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     }

#undef  WAIT

/*------------------------------------- Фиксируем выходное состояние */

                     x_prv=this->state.x ;
                     y_prv=this->state.y ;
                     z_prv=this->state.z ;
                     a_prv=this->state.azim ;
                     e_prv=this->state.elev ;
                     r_prv=this->state.roll ;
                     v_prv=this->v_abs ;
                     g_prv=this->g_ctrl ;

/*-------------------------------------------------------------------*/

#undef   FRAME

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   Сохранение точки траектории                     */

  int  RSS_Object_Flyer::iSaveTracePoint(char *action)

{
/*------------------------------------------------- Сброс траектории */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*-------------------------------------- Контроль режима отображения */

   if(mTrace_flag==0)  return(0) ;

/*----------------------------------------------- Довыделение буфера */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_FlyerTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_FlyerTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("FLYER.iSaveTracePoint@"
                              "Memory over for trajectory") ;
                                  return(-1) ;
                        }
                              }
/*------------------------------------------------- Сохранение точки */
                  
         mTrace[mTrace_cnt].x_base    =this->state.x ;
         mTrace[mTrace_cnt].y_base    =this->state.y ;
         mTrace[mTrace_cnt].z_base    =this->state.z ;
         mTrace[mTrace_cnt].a_azim    =this->state.azim ;
         mTrace[mTrace_cnt].a_elev    =this->state.elev ;
         mTrace[mTrace_cnt].a_roll    =this->state.roll ;
         mTrace[mTrace_cnt].x_velocity=this->state.x_velocity ;
         mTrace[mTrace_cnt].y_velocity=this->state.y_velocity ;
         mTrace[mTrace_cnt].z_velocity=this->state.z_velocity ;
                mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           Отображение траектории с передачей контекста           */

  void  RSS_Object_Flyer::iShowTrace_(void)

{
   if(mTrace_flag==0)  return ;


       strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*								     */
/*                     Отображение траектории                        */

  void  RSS_Object_Flyer::iShowTrace(void)

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

             glColor4d(GetRValue(mTrace_color)/256., 
                       GetGValue(mTrace_color)/256.,
                       GetBValue(mTrace_color)/256., 1.) ;

               glBegin(GL_LINE_STRIP) ;

       for(i=0 ; i<mTrace_cnt ; i++)
            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;

                  glEnd();

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

     RSS_Transit_Flyer::RSS_Transit_Flyer(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Flyer::~RSS_Transit_Flyer(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Flyer::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_Flyer *)object)->iShowTrace() ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "ПРОГРАММА УПРАВЛЕНИЯ"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ПРОГРАММА УПРАВЛЕНИЯ"      	     */

     RSS_Object_FlyerProgram::RSS_Object_FlyerProgram(void)

{
    memset(name, 0, sizeof(name)) ;
           frames_cnt=0 ;
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ПРОГРАММА УПРАВЛЕНИЯ"      	     */

    RSS_Object_FlyerProgram::~RSS_Object_FlyerProgram(void)

{
}



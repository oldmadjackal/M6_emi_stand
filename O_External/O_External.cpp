/********************************************************************/
/*                                                                  */
/*           МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ВНЕШНЯЯ МОДЕЛЬ"            */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <winsock.h>
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

#include "..\tcp_lib\exh.h"
#include "..\tcp_lib\tcp.h"

#include "O_External.h"

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
/*                                                                  */
/*                       Программный модуль                         */

     static   RSS_Module_External  ProgramModule ;


/********************************************************************/
/*                                                                  */
/*                     Идентификационный вход                       */

 O_EXTERNAL_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_EXTERNAL_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**                                                                **/
/**   ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "ВНЕШНЯЯ МОДЕЛЬ"  **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*                                                                  */
/*                            Список команд                         */

  struct RSS_Module_External_instr  RSS_Module_External_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_External::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_External::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_External::cInfo },
 { "copy",    "cp", "#COPY - копировать объект",
                    " COPY <Имя образца> <Имя нового объекта>\n"
                    "   Копировать объект",
                    &RSS_Module_External::cCopy },
 { "owner",   "o",  "#OWNER - назначить носитель ракеты",
                    " OWNER <Имя> <Носитель>\n"
                    "   Назначить объект - носитель ракеты",
                    &RSS_Module_External::cOwner },
 { "target",  "tg", "#TARGET - назначить цель ракеты",
                    " TARGET <Имя> <Цель>\n"
                    "   Назначить объект - цель ракеты",
                    &RSS_Module_External::cTarget },
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
                    &RSS_Module_External::cBase },
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
                    &RSS_Module_External::cAngle },
 { "velocity", "v", "#VELOCITY - задание скорости движения объекта",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задание абсолютного значения скорости объекта"
                    " VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>\n"
                    "   Задание проекций скорости объекта",
                    &RSS_Module_External::cVelocity },
 { "control",  "c", "#CONTROL - управление объектом",
                    " CONTROL <Имя> <Угол крена> [<Перегрузка>]\n"
                    "   Задание полного набора параметров управления\n"
                    " CONTROL/G <Имя> <Перегрузка>\n"
                    "   Задание полного набора параметров управления\n"
                    " CONTROL> <Имя> [<Шаг крена> [<Шаг ускорения>]]\n"
                    "   Управление стрелочками\n",
                    &RSS_Module_External::cControl },
 { "trace",    "t", "#TRACE - трассировка траектории объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Трассировка траектории объекта в реальном времени\n",
                    &RSS_Module_External::cTrace },
 { "itype",   "it", "#ITYPE - задание типа интерфейса с внешним модулем",
                    " ITYPE <Имя>\n"
                    "   Задает параметры интерфейса в диалоговом режиме\n"
                    " ITYPE <Имя> <iface_type> [<object_type>]\n"
                    "   Задает тип интерфейса (file, tcp-server) и тип объекта\n",
                    &RSS_Module_External::cIType },
 { "ifile",   "if", "#IFILE - параметры файлового интерфейса с внешним модулем",
                    " IFILE <Имя>\n"
                    "   Задает параметры в диалоговом режиме\n"
                    " IFILE/f <Имя> <path>\n"
                    "   Задает путь папки обменных файлов\n"
                    " IFILE/t <Имя> <path>\n"
                    "   Задает имя файла списка объектов сцены\n"
                    " IFILE/c <Имя> <name>\n"
                    "   Задает имя для входного/выходного файлов данных\n",
                    &RSS_Module_External::cIFile },
 { "itcp",    "ic", "#ITCP - параметры сетевого интерфейса с внешним модулем",
                    " ITCP <Имя>\n"
                    "   Задает параметры в диалоговом режиме\n"
                    " ITCP/u <Имя> <URL>\n"
                    "   Задает URL внешнего модуля\n",
                    &RSS_Module_External::cITcp },
 {  NULL }
                                                            } ;

/********************************************************************/
/*                                                                  */
/*                       Общие члены класса                         */

    struct RSS_Module_External_instr *RSS_Module_External::mInstrList=NULL ;


/********************************************************************/
/*                                                                  */
/*                        Конструктор класса                        */

     RSS_Module_External::RSS_Module_External(void)

{
	   keyword="EmiStand" ;
    identification="External" ;
          category="Object" ;

        mInstrList=RSS_Module_External_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*                                                                  */
/*                         Деструктор класса                        */

    RSS_Module_External::~RSS_Module_External(void)

{
}


/********************************************************************/
/*                                                                  */
/*                        Создание объекта                          */

  RSS_Object *RSS_Module_External::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_External *object ;
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
              SEND_ERROR("Секция EXTERNAL: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция EXTERNAL: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция EXTERNAL: Не задана модель объекта") ;
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
              SEND_ERROR("Секция EXTERNAL: Неизвестная модель тела") ;
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

              SEND_ERROR("Секция EXTERNAL: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_External ;
    if(object==NULL) {
              SEND_ERROR("Секция EXTERNAL: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }
/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция EXTERNAL: Переполнение памяти") ;
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
                         SEND_ERROR("Секция EXTERNAL: Переполнение памяти") ;
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
                                         data->sections[i].decl, "External.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция EXTERNAL: Переполнение памяти") ;
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
/*                                                                  */
/*                         Получить параметр                        */

     int  RSS_Module_External::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Внешняя модель", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                         Выполнить команду                        */

  int  RSS_Module_External::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "EXTERNAL"
#define  _SECTION_SHRT_NAME   "EXTERNAL"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object External:") ;
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
       if(status)  SEND_ERROR("Секция EXTERNAL: Неизвестная команда") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*                                                                  */
/*                     Считать данные из строки                     */

    void  RSS_Module_External::vReadSave(std::string *data)

{
                char *buff ;
                 int  buff_size ;
      RSS_Model_data  create_data ;
 RSS_Object_External *object ;
                char  name[128] ;
                char *entry ;
                char *end ;
                 int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE EXTERNAL\n", 
                      strlen("#BEGIN MODULE EXTERNAL\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT EXTERNAL\n", 
                      strlen("#BEGIN OBJECT EXTERNAL\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT EXTERNAL\n", 
              strlen("#BEGIN OBJECT EXTERNAL\n"))) {                /* IF.1 */
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
                object=(RSS_Object_External *)vCreateObject(&create_data) ;
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
                                                 }                  /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                     Записать данные в строку                     */

    void  RSS_Module_External::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE EXTERNAL\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции HELP                    */

  int  RSS_Module_External::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_External_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 Реализация инструкции CREATE                     */
/*                                                                  */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_External::cCreate(char *cmd)

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
                                     Object_External_Create_dialog, 
                                    (LPARAM)&data               ) ;
   if(status)  return(-1) ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции INFO                    */
/*                                                                  */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_External::cInfo(char *cmd)

{
                char  *name ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*		      Реализация инструкции COPY                    */
/*								    */
/*       COPY <Имя образца> <Имя нового объекта>                    */

  int  RSS_Module_External::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *copy ;
   RSS_Object_External  *sample ;
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

       sample=(RSS_Object_External *)FindObject(name, 1) ;        /* Ищем объект-цель по имени */
    if(sample==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(copy==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-копии. \n"
                                 "Например: COPY <Имя образца> <Имя нового объекта>") ;
                                     return(-1) ;
                   }

//     object=FindObject(copy, 0) ;                                 /* Ищем объект-носитель по имени */
//  if(object!=NULL) {
//                    SEND_ERROR("Oбъект-копия уже существует") ;
//                                   return(-1) ;
//                   }
/*---------------------------------------------- Копирование объекта */

            object=sample->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   Реализация инструкции OWNER                    */
/*                                                                  */
/*       OWNER <Имя> <Носитель>                                     */

  int  RSS_Module_External::cOwner(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *owner ;
 RSS_Object_External  *e_object ;
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

       e_object=(RSS_Object_External *)FindObject(name, 1) ;        /* Ищем объект-цель по имени */
    if(e_object==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(owner==NULL) {                                               /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-носителя. \n"
                                 "Например: OWNER <Имя_ракеты> <Имя_носителя>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* Ищем объект-носитель по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Пропись носителя */

          strcpy(e_object->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   Реализация инструкции TARGET                   */
/*                                                                  */
/*       TARGET <Имя> <Цель>                                        */

  int  RSS_Module_External::cTarget(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *target ;
 RSS_Object_External  *e_object ;
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

       e_object=(RSS_Object_External *)FindObject(name, 1) ;        /* Ищем объект-цель по имени */
    if(e_object==NULL)  return(-1) ;

/*---------------------------------------------- Контроль имени цели */

    if(target==NULL) {                                              /* Если имя не задано... */
                        SEND_ERROR("Не задано имя объекта-цели. \n"
                                   "Например: TARGET <Имя_ракеты> <Имя_цели>") ;
                                     return(-1) ;
                    }

       object=FindObject(target, 0) ;                               /* Ищем объект-цели по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------------- Пропись цели */

               strcpy(e_object->target, target) ;

   for(i=0 ; i<e_object->Features_cnt ; i++)
     if(!stricmp(e_object->Features[i]->Type, "Hit")) 
         strcpy(((RSS_Feature_Hit *)(e_object->Features[i]))->target, target) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции BASE                    */
/*                                                                  */
/*        BASE    <Имя> <X> <Y> <Z>                                 */
/*        BASE/X  <Имя> <X>                                         */
/*        BASE/+X <Имя> <X>                                         */
/*        BASE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        BASE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_External::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*                                                                  */
/*                    Реализация инструкции ANGLE                   */
/*                                                                  */
/*       ANGLE    <Имя> <A> <E> <R>                                 */
/*       ANGLE/A  <Имя> <A>                                         */
/*       ANGLE/+A <Имя> <A>                                         */
/*       ANGLE>   <Имя> <Код стрелочки> <Шаг>                       */
/*       ANGLE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_External::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*                                                                  */
/*                    Реализация инструкции VELOCITY                */
/*                                                                  */
/*       VELOCITY <Имя> <Скорость>                                  */
/*       VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>      */

  int  RSS_Module_External::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*                                                                  */
/*                 Реализация инструкции CONTROL                    */
/*                                                                  */
/*       CONTROL   <Имя> <Угол крена> [<Перегрузка>]                */
/*       CONTROL/G <Имя> <Перегрузка>                               */
/*       CONTROL>  <Имя> <Код стрелочки>                            */
/*       CONTROL>> <Имя> <Код стрелочки>                            */

  int  RSS_Module_External::cControl(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*                                                                  */
/*                   Реализация инструкции TRACE                    */
/*                                                                  */
/*       TRACE <Имя> [<Длительность>]                               */

  int  RSS_Module_External::cTrace(char *cmd)

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
 RSS_Object_External *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
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
/*                                                                  */
/*                 Реализация инструкции ITYPE                      */
/*                                                                  */
/*       ITYPE <Имя>                                                */
/*       ITYPE <Имя> <Iface type> [<Object type>]                   */

  int  RSS_Module_External::cIType(char *cmd)

{
#define   _PARS_MAX  10

                char *pars[_PARS_MAX] ;
                char *name ;
 RSS_Object_External *object ;
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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ITYPE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------- Задание через диалог */

   if(pars[1]==NULL || *pars[1]==0) {

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_IFACE", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_External_Iface_dialog, 
                                      (LPARAM)object               ) ;
         return((int)status) ;
                                    }
/*------------------------------------------ Задание типа интерфейса */

                     strupr(pars[1]) ;

   if(stricmp(pars[1], "FILE"      ) &&
      stricmp(pars[1], "TCP-SERVER")   ) {

              SEND_ERROR("Допустимы следующие типы интерфейса: FILE, TCP-SERVER") ;
                     return(-1) ;
                                         }

                strcpy(object->iface_type, pars[1]) ;

   if(pars[2]!=NULL && *pars[2]!=0)
                strcpy(object->object_type, pars[2]) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции IFILE                   */
/*                                                                  */
/*       IFILE/F <Имя> <Folder path>                                */
/*       IFILE/T <Имя> <Objects path>                               */
/*       IFILE/C <Имя> <Controls name>                              */

  int  RSS_Module_External::cIFile(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
 RSS_Object_External  *object ;
                 int   f_flag, t_flag, c_flag ;
                char  *end ;
                 int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      f_flag=0 ;
                      t_flag=0 ;
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

                if(strchr(cmd, 'f')!=NULL ||
                   strchr(cmd, 'F')!=NULL   )  f_flag=1 ;
           else if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  t_flag=1 ;
           else if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  c_flag=1 ;

                           cmd=end+1 ;
                        }

       if(f_flag+t_flag+c_flag==0) {
                      SEND_ERROR("Должен быть задан хотя бы один из ключей: F, T или C. \n"
                                 "Например: IFILE/F <Имя_объекта> ...") ;
                                           return(-1) ;
                                   }
       else
       if(f_flag+t_flag+c_flag!=1) {
                      SEND_ERROR("Должен быть только один из ключей: F, T или C. \n"
                                 "Например: IFILE/F <Имя_объекта> ...") ;
                                           return(-1) ;
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

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: IFILE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------------- Пропись данных */

    if(pars[1]==NULL) {
                          SEND_ERROR("Не задано значение параметра") ;
                                             return(-1) ;
                      }

    if(f_flag) {

            if(access(pars[1], 0x00)!=0) {
                      SEND_ERROR("Указана несуществующая папка") ;
                                             return(-1) ;
                                         }

                  strcpy(object->iface_file_folder, pars[1]) ;
               }
    else 
    if(t_flag) {
                  strcpy(object->iface_targets, pars[1]) ;
               }
    else 
    if(c_flag) {
                  strcpy(object->iface_file_control, pars[1]) ;
               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции ITCP                    */
/*                                                                  */
/*       ITCP/U <Имя> <URL>                                         */

  int  RSS_Module_External::cITcp(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
 RSS_Object_External  *object ;
                 int   u_flag ;
                char  *end ;
                 int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      u_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'u')!=NULL ||
                   strchr(cmd, 'U')!=NULL   )  u_flag=1 ;

                           cmd=end+1 ;
                        }

       if(u_flag/*+t_flag*/==0) {
                         SEND_ERROR("Должен быть задан хотя бы один из ключей: U ... \n"
                                    "Например: ITCP/U <Имя_объекта> ...") ;
                                           return(-1) ;
                                }
       else
       if(u_flag/*+t_flag*/!=1) {
                         SEND_ERROR("Должен быть только один из ключей: U ... \n"
                                    "Например: ITCP/U <Имя_объекта> ...") ;
                                           return(-1) ;
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

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ITCP <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------------- Пропись данных */

    if(pars[1]==NULL) {
                          SEND_ERROR("Не задано значение параметра") ;
                                             return(-1) ;
                      }

    if(u_flag) {
                  strcpy(object->iface_tcp_connect, pars[1]) ;
                  strcpy(object->iface_targets,     pars[1]) ;
               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                Поиск обьекта типа EXTERNAL по имени              */

  RSS_Object *RSS_Module_External::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "External")) {

           SEND_ERROR("Объект не является объектом типа EXTERNAL") ;
                            return(NULL) ;
                                              }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**                                                                **/
/**          ОПИСАНИЕ КЛАССА ОБЪЕКТА "ВНЕШНЯЯ МОДЕЛЬ"              **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Статические переменные			    */

 O_EXTERNAL_API                            char *RSS_Object_External::targets     =NULL ;
 O_EXTERNAL_API                          double  RSS_Object_External::targets_time=  0. ;
 O_EXTERNAL_API                             int  RSS_Object_External::targets_init=  0 ;
 O_EXTERNAL_API  struct RSS_Object_ExternalLink *RSS_Object_External::targets_links[_TARGETS_LINKS_MAX] ;
 O_EXTERNAL_API                             int  RSS_Object_External::targets_links_cnt ;


/********************************************************************/
/*                                                                  */
/*                       Конструктор класса                         */

     RSS_Object_External::RSS_Object_External(void)

{
   strcpy(Type, "External") ;

   CalculateExt_use= 1 ;

    Context        =new RSS_Transit_External ;
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

           start_flag=0 ;

    strcpy(object_type,        "") ;

    strcpy(iface_type,         "") ;
    strcpy(iface_file_folder,  "") ;
    strcpy(iface_file_control, "$NAME$") ;
    strcpy(iface_tcp_connect,  "") ;
    strcpy(iface_targets,      "") ;

      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=RGB(0, 0, 127) ;
      mTrace_dlist=  0 ;  
}


/********************************************************************/
/*                                                                  */
/*                          Деструктор класса                       */

    RSS_Object_External::~RSS_Object_External(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*                                                                  */
/*                     Освобождение ресурсов                        */

  void   RSS_Object_External::vFree(void)

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
/*                                                                  */
/*                    Сохранить состояние объекта                   */
/*                    Восстановить состояние объекта                */

    void  RSS_Object_External::vPush(void)

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


    void  RSS_Object_External::vPop(void)

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
/*                        Копировать объект		            */

    class RSS_Object *RSS_Object_External::vCopy(char *name)

{
        RSS_Model_data  create_data ;
   RSS_Object_External *object ;
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

       object=(RSS_Object_External *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;

            strcpy(object->object_type,        this->object_type) ;

            strcpy(object->iface_type,         this->iface_type) ;
            strcpy(object->iface_file_folder,  this->iface_file_folder) ;
            strcpy(object->iface_file_control, this->iface_file_control) ;
            strcpy(object->iface_tcp_connect,  this->iface_tcp_connect) ;
            strcpy(object->iface_targets,      this->iface_targets) ;

   if(RSS_Kernel::battle)  object->battle_state=_SPAWN_STATE ;

/*---------------------------------------------- Копирование свойств */

/*
    for(i=0 ; i<this->Features_cnt ; i++)
      if(!stricmp(this->Features[i]->Type, "Hit"))
             hit_1=(RSS_Feature_Hit *)this->Features[i] ;

    for(i=0 ; i<object->Features_cnt ; i++)
      if(!stricmp(object->Features[i]->Type, "Hit"))
             hit_2=(RSS_Feature_Hit *)object->Features[i] ;

           hit_2->hit_range =hit_1->hit_range ;
           hit_2->any_target=hit_1->any_target ;
           hit_2->any_weapon=hit_1->any_weapon ;
*/
/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*                                                                  */
/*                      Записать данные в строку                    */

    void  RSS_Object_External::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT EXTERNAL\n" ;

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
/*                                                                  */
/*                        Обработка событий                         */

     int  RSS_Object_External::vEvent(char *event_name, double  t, char *callback, int cb_size)
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
/*                                                                  */
/*                        Специальные действия                      */

     int  RSS_Object_External::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*                                                                  */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_External::vCalculateStart(double  t)
{
     Matrix2d  Sum_Matrix ;
     Matrix2d  Oper_Matrix ;  
     Matrix2d  Velo_Matrix ;
         char  flag[FILENAME_MAX] ;
         char  name[128] ;
         WORD  version ;
      WSADATA  winsock_data ;        /* Данные системы WINSOCK */
          int  status ;
         char  text[1024] ;
          int  i ; 

  static  int  sockets_init ;

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*------------------------------------------ Инициализация стека TCP */

   if(sockets_init==0) {

                          version=MAKEWORD(1, 1) ;
        status=WSAStartup(version, &winsock_data) ;                 /* Иниц. Win-Sockets */
     if(status) {
                     sprintf(text, "Win-socket DLL loading error: %d", WSAGetLastError()) ;
                  SEND_ERROR(text) ;
                       return(-1) ;
                }

                       }

      sockets_init=1 ;

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

/*----------------------- Очистка обменных данных для внешней модели */

   if(this->CalculateExt_use==1) {
/*- - - - - - - - - - - - - - - - - - -  Очистка для интерфейса FILE */
     if(!stricmp(this->iface_type, "FILE"      )) {

                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

            sprintf(flag, "%s/%s.out.flag", this->iface_file_folder, name) ;
             unlink(flag) ;
            sprintf(flag, "%s/%s.in.flag", this->iface_file_folder, name) ;
             unlink(flag) ;

                                                  }
/*- - - - - - - - - - - - - - - -  Очистка для интерфейса TCP-SERVER */
     else
     if(!stricmp(this->iface_type, "TCP-SERVER")) {


                                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }  
/*------------------------------- Установка признака начального шага */

            this->start_flag=1 ;

/*------------------------------------------------ Очистка контекста */

            this->iSaveTracePoint("CLEAR") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_External::vCalculate(double t1, double t2, char *callback, int cb_size)
{
/*----------------------------------------------- Без внешней модели */

                   x_base+=x_velocity*(t2-t1) ;
                   y_base+=y_velocity*(t2-t1) ;
                   z_base+=z_velocity*(t2-t1) ;
                   g_over = 0 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           Расчет изменения состояния во внешней модели           */

     int  RSS_Object_External::vCalculateExt1(double t1, double t2, char *callback, int callback_size)
{
        char  name[128] ;
        char  flag[FILENAME_MAX] ;
        char  path[FILENAME_MAX] ;
        FILE *file ;
         Tcp  tcp_server_iface ;
  Tcp_packet  data ;
         int  status ;
        char  value[128] ;
        char  text[4096] ;
        char *end ;
         int  i ;

/*------------------------------------- Формирование списка объектов */

#define   OBJECTS       this->Module->kernel_objects 
#define   OBJECTS_CNT   this->Module->kernel_objects_cnt 

    if(targets_init==1) {
                            targets_time=-1. ;
                            targets_init= 0 ;
                        }
    if(targets_time<t1) {

       if(targets==NULL)   targets=(char *)calloc(1, _TARGETS_MAX) ;

                          *targets=0 ;

        sprintf(text, "\"t1\":\"%.2lf\";\"t2\":\"%.2lf\";[\n", t1, t2) ;
         strcat(targets, text) ;  

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(OBJECTS[i]->battle_state) {

#define   O    OBJECTS[i]

             sprintf(text, "{ \"name\":\"%s\";"
                             "\"x\":\"%.2lf\";\"y\":\"%.2lf\";\"z\":\"%.2lf\";"
                             "\"azim\":\"%.2lf\";\"elev\":\"%.2lf\";\"roll\":\"%.2lf\";"
                             "\"v_x\":\"%.2lf\";\"v_y\":\"%.2lf\";\"v_z\":\"%.2lf\"};\n",
                                O->Name,
                                O->x_base,     O->y_base,     O->z_base,
                                O->a_azim,     O->a_elev,     O->a_roll,
                                O->x_velocity, O->y_velocity, O->z_velocity  ) ;

              strcat(targets, text) ;  

#undef    O
                                      }

        sprintf(text, "{\"name\":\"$END_OF_LIST$\"}\n]\n") ;
         strcat(targets, text) ;  

                                  targets_time=t1 ;
                        } 

#undef   OBJECTS
#undef   OBJECTS_CNT

/*----------------------------------------- Отправка списка объектов */

   do {
/*- - - - - - - - - - - - - - - - - - -  Проверка повторной отправки */
         for(i=0 ; i<targets_links_cnt ; i++)
           if(!strcmp(targets_links[i]->link, this->iface_targets))  break ;

           if(i<targets_links_cnt) {
                  if(targets_links[i]->time_mark==t1)  break ;      /* Если список объектов по данной ссылке уже формировался */ 
                                   }
           else                    {
                       targets_links[i]=(struct RSS_Object_ExternalLink *)calloc(1, sizeof(struct RSS_Object_ExternalLink)) ;
                strcpy(targets_links[i]->link, this->iface_targets) ;
                       targets_links_cnt++ ;
                                   }

                     targets_links[i]->time_mark=t1 ;
/*- - - - - - - - - - - - - -  Отправка запроса через интерфейс FILE */
   if(!stricmp(this->iface_type, "FILE")) {

         file=fopen(this->iface_targets, "w") ;
      if(file==NULL) {
                          sprintf(text, "Ошибка создания файла объектов: %s", this->iface_targets) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fwrite(targets, 1, strlen(targets), file) ;
             fclose(file) ;

                                          }
/*- - - - - - - - - - -  Отправка запроса через интерфейс TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_targets) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "Объект %s - Ошибка формата URL списка целей", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

           memmove(targets+8, targets, strlen(targets)+1) ;         /* Добавляем префикс типа сообщения */
            memcpy(targets, "TARGETS:", 8) ;

                                         data.data_out.assign(targets) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "Объект %s - Ошибка отправки TARGETS-запроса %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

       if(stricmp(data.data_in.c_str(), "SUCCESS")) {               /* Проверка результата */

                          sprintf(text, "Объект %s - Ошибка исполнения TARGETS-запроса: %s", this->Name, data.data_in.c_str()) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                    }

                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(0) ;

/*------------------------------- Отправка запроса на рассчет модели */
/*- - - - - - - - - - - - - - - - - - - - - - - Формирование запроса */
         memset(text, 0, sizeof(text)) ;

        sprintf(value, "\"name\":\"%s\";\"start\":\"%d\";\"t1\":\"%.2lf\";\"t2\":\"%.2lf\"\n", this->Name, this->start_flag, t1, t2) ;
         strcat(text, value) ;  
        sprintf(value, "\"x\":\"%.2lf\";\"y\":\"%.2lf\";\"z\":\"%.2lf\"\n", this->x_base, this->y_base, this->z_base) ;
         strcat(text, value) ;  
        sprintf(value, "\"azim\":\"%.2lf\";\"elev\":\"%.2lf\";\"roll\":\"%.2lf\"\n", this->a_azim, this->a_elev, this->a_roll) ;
         strcat(text, value) ;  
        sprintf(value, "\"v_x\":\"%.2lf\";\"v_y\":\"%.2lf\";\"v_z\":\"%.2lf\"\n", this->x_velocity, this->y_velocity, this->z_velocity) ;
         strcat(text, value) ;  
        sprintf(value, "\"type\":\"%s\";\"target\":\"%s\"\n", this->object_type, this->target) ;
         strcat(text, value) ;  
/*- - - - - - - - - - - - - -  Отправка запроса через интерфейс FILE */
   if(!stricmp(this->iface_type, "FILE")) {
                         
                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

            sprintf(path, "%s/%s.out",      this->iface_file_folder, name) ;
            sprintf(flag, "%s/%s.out.flag", this->iface_file_folder, name) ;

         file=fopen(path, "w") ;
      if(file==NULL) {
                          sprintf(text, "Объект %s - Ошибка создания файла запроса: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fwrite(text, 1, strlen(text), file) ;
             fclose(file) ;

         file=fopen(flag, "w") ;
      if(file==NULL) {
                          sprintf(text, "Объект %s - Ошибка создания флаг-файла запроса: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fclose(file) ;
 
                                          }
/*- - - - - - - - - - -  Отправка запроса через интерфейс TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_tcp_connect) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "Объект %s - Ошибка формата URL соединения", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

           memmove(text+5, text, strlen(text)+1) ;                  /* Добавляем префикс типа сообщения */
            memcpy(text, "STEP:", 5) ;

                                         data.data_out.assign(text) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "Объект %s - Ошибка отправки STEP-запроса %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

       if(stricmp(data.data_in.c_str(), "SUCCESS")) {               /* Проверка результата */

                          sprintf(text, "Объект %s - Ошибка исполнения STEP-запроса: %s", this->Name, data.data_in.c_str()) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                    }

                                                }
/*----------------------------------- Сброс признака начального шага */

                  this->start_flag=0 ;

/*-------------------------------------------------------------------*/
    
  return(0) ;
}

     int  RSS_Object_External::vCalculateExt2(double t1, double t2, char *callback, int callback_size)
{
        char  name[128] ;
        char  flag[FILENAME_MAX] ;
        char  path[FILENAME_MAX] ;
        FILE *file ;
         Tcp  tcp_server_iface ;
  Tcp_packet  data ;
        char  em_name[128] ;
      double  em_t1 ;
      double  em_t2 ;
        char  em_message[1024] ;
         int  status ;
        char  value[1024] ;
        char  text[4096] ;
        char *key ;
        char *end ;
         int  i ;

  static  char *keys[]={"\"name\":\"", "\"t1\":\"",   "\"t2\":\"",
                        "\"x\":\"",    "\"y\":\"",    "\"z\":\"",
                        "\"azim\":\"", "\"elev\":\"", "\"roll\":\"",
                        "\"v_x\":\"",  "\"v_y\":\"",  "\"v_z\":\"",
                        "\"message\":\"",
                         NULL} ;

/*------------------------------------------ Ожидание расчета модели */

   do {
/*- - - - - - - - - - - - - -  Отправка запроса через интерфейс FILE */
   if(!stricmp(this->iface_type, "FILE")) {

                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

        sprintf(path, "%s/%s.in",      this->iface_file_folder, name) ;
        sprintf(flag, "%s/%s.in.flag", this->iface_file_folder, name) ;

      if(access(flag, 0x00)) continue ;

         file=fopen(path, "rt") ;
      if(file==NULL) {
                          sprintf(text, "Объект %s - Ошибка открытия файла ответа: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             memset(text, 0, sizeof(text)) ;
              fread(text, 1, sizeof(text)-1, file) ;
             fclose(file) ;

        status=unlink(flag) ;
      if(status) {
                          sprintf(text, "ERROR - Response flag-file remove error %d : %s", errno, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                 }

                      break ;
                                          }
/*- - - - - - - - - - -  Отправка запроса через интерфейс TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_tcp_connect) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "Объект %s - Ошибка формата URL соединения", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

          sprintf(text, "GET:\"name\":\"%s\";\"t1\":\"%.2lf\";",   /* Формирование запроса */
                            this->Name, t1) ;

                                         data.data_out.assign(text) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "Объект %s - Ошибка отправки GET-запроса %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

            memset(text, 0, sizeof(text)) ;
           strncpy(text, data.data_in.c_str(), sizeof(text)-1) ;

       if(!memicmp(text, "WARNING", strlen("WARNING"))) {           /* Результат ещё не готов */
                         continue ;
                                                        }
       else
       if(!memicmp(text, "ERROR",   strlen("ERROR"  ))) {           /* Ошибка */

                           memset(value, 0, sizeof(value)) ;
                          strncpy(value, text+6, sizeof(value)-1) ;
                          sprintf(text, "Объект %s - Ошибка исполнения GET-запроса: %s", this->Name, value) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                        }
       else
       if(!memicmp(text, "SUCCESS", strlen("SUCCESS"))) {           /* Получен результат */

              memmove(text, text+8, strlen(text+8)+1) ;
                              break ;
                                                        }
       else                                             {

                           memset(value, 0, sizeof(value)) ;
                          strncpy(value, text, sizeof(value)-1) ;
                          sprintf(text, "Объект %s - Некорректный ответ на GET-запрос: %s", this->Name, value) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                        }
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
             Sleep(10) ;

      } while(1) ;

/*-------------------------------------------- Разбор данных расчета */

   for(i=0 ; keys[i]!=NULL ; i++) {

        key=strstr(text, keys[i]) ;
     if(key==NULL) {
                         sprintf(text, "Parameter %s is missed", keys[i]) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                   } 

            memset(value, 0, sizeof(value)) ;
           strncpy(value, key+strlen(keys[i]), sizeof(value)-1) ;
        end=strchr(value, '"')  ;
     if(end==NULL) {
                         sprintf(text, "Invalid value for parameter %s", keys[i]) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                   } 

       *end=0 ;

     if(i== 0)  strcpy(em_name, value) ;  
     if(i== 1)         em_t1           =strtod(value, &end) ;
     if(i== 2)         em_t2           =strtod(value, &end) ;
     if(i== 3)         this->x_base    =strtod(value, &end) ;
     if(i== 4)         this->y_base    =strtod(value, &end) ;
     if(i== 5)         this->z_base    =strtod(value, &end) ;
     if(i== 6)         this->a_azim    =strtod(value, &end) ;
     if(i== 7)         this->a_elev    =strtod(value, &end) ;
     if(i== 8)         this->a_roll    =strtod(value, &end) ;
     if(i== 9)         this->x_velocity=strtod(value, &end) ;
     if(i==10)         this->y_velocity=strtod(value, &end) ;
     if(i==11)         this->z_velocity=strtod(value, &end) ;
     if(i==12)    strcpy(em_message, value) ;  

                                  } 
/*------------------------------- Формирование команд обратной связи */

  while(em_message[0]!=0) {

         end=strchr(em_message, ';') ;
      if(end==NULL) {
                         sprintf(text, "Command element terminator ';' missed") ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                    }             

        *end=0 ;

#define  _KEY  "DESTROY"

   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "KILL %s;", this->Name) ;

                                                }

#undef   _KEY
#define  _KEY  "START"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "%s;", em_message) ;

                                                }

#undef   _KEY
#define  _KEY  "STOP"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "STOP %s;", this->Name) ;

                                                }

#undef   _KEY
#define  _KEY  "HIT"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "EVENT HITED %s;", em_message+strlen(_KEY)+1) ;

                                                }

#undef   _KEY
#define  _KEY  "BLAST-A"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

             strcpy(name, em_message+strlen(_KEY)+1) ;
            sprintf(value, "EXEC BLAST CR/A blast_%s 0 10 %s;START blast_%s;", name, name, name) ;

                                                }

#undef   _KEY
#define  _KEY  "EXEC"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "%s;", em_message) ;

                                                }
   else                                         {

            sprintf(text, "Unknown elements in command: %s", em_message) ;
         SEND_ERROR(text) ;
              return(-1) ;
                                                }

      if(strlen(callback)+strlen(value)>=callback_size-8) {
                         sprintf(text, "Callback buffer overflow") ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                                                          }

                        strcat(callback, value) ;
                       memmove(em_message, end+1, strlen(end+1)+1) ;
                 } 

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_External::vCalculateShow(double  t1, double  t2)
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
/*                                                                   */
/*                   Сохранение точки траектории                     */

  int  RSS_Object_External::iSaveTracePoint(char *action)

{
/*------------------------------------------------- Сброс траектории */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- Довыделение буфера */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_ExternalTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_ExternalTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("EXTERNAL.iSaveTracePoint@"
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
/*                                                                  */
/*           Отображение траектории с передачей контекста           */

  void  RSS_Object_External::iShowTrace_(void)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Отображение траектории                        */

  void  RSS_Object_External::iShowTrace(void)

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
/*                                                                   */
/*              Компоненты класса "ТРАНЗИТ КОНТЕКСТА"                */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*                                                                   */
/*              Конструктор класса "ТРАНЗИТ КОНТЕКСТА"               */

     RSS_Transit_External::RSS_Transit_External(void)

{
}


/*********************************************************************/
/*                                                                   */
/*               Деструктор класса "ТРАНЗИТ КОНТЕКСТА"               */

    RSS_Transit_External::~RSS_Transit_External(void)

{
}


/********************************************************************/
/*                                                                  */
/*                    Исполнение действия                           */

    int  RSS_Transit_External::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_External *)object)->iShowTrace() ;

   return(0) ;
}

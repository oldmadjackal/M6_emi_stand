/********************************************************************/
/*                                                                  */
/*     МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЗЕНИТНЫЙ РАКЕТНЫЙ КОМПЛЕКС"      */
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

#include "O_ZRK.h"

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

     static   RSS_Module_ZRK  ProgramModule ;


/********************************************************************/
/*                                                                  */
/*                     Идентификационный вход                       */

 O_ZRK_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_ZRK_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**                                                                **/
/**         ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ             **/
/**                "ЗЕНИТНЫЙ РАКЕТНЫЙ КОМПЛЕКС"                    **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*                                                                  */
/*                            Список команд                         */

  struct RSS_Module_ZRK_instr  RSS_Module_ZRK_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_ZRK::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_ZRK::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_ZRK::cInfo },
 { "copy",    "cp", "#COPY - копировать объект",
                    " COPY <Имя образца> <Имя нового объекта>\n"
                    "   Копировать объект",
                    &RSS_Module_ZRK::cCopy },
 { "owner",   "o",  "#OWNER - назначить носитель",
                    " OWNER <Имя> <Носитель>\n"
                    "   Назначить объект - носитель ракеты",
                    &RSS_Module_ZRK::cOwner },
 { "target",  "tg", "#TARGET - назначить цель",
                    " TARGET <Имя> <Цель>\n"
                    "   Назначить объект - цель ракеты",
                    &RSS_Module_ZRK::cTarget },
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
                    &RSS_Module_ZRK::cBase },
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
                    &RSS_Module_ZRK::cAngle },
 { "velocity", "v", "#VELOCITY - задание скорости движения объекта",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задание абсолютного значения скорости объекта"
                    " VELOCITY <Имя> <X-скорость> <Y-скорость> <Z-скорость>\n"
                    "   Задание проекций скорости объекта",
                    &RSS_Module_ZRK::cVelocity },
 { "trace",    "t", "#TRACE - трассировка траектории объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Трассировка траектории объекта в реальном времени\n",
                    &RSS_Module_ZRK::cTrace },
 { "firing",   "f", "#FIRING - параметры боевой работы",
                    " FIRING <Имя>\n"
                    "   Задает параметры в диалоговом режиме\n"
                    " FIRING/r <Имя> <Дальность-min> <Дальность-max> <Высота-max>\n"
                    "   Задает параметры рубежа открытия огня\n"
                    " FIRING/b <Имя> <Тип цели>\n"
                    "   Задает тип цели: 0 - аэродинамическая, 1 - баллистическая\n"
                    " FIRING/m <Имя> <Объект-ракета> <Число ракет>\n"
                    "   Задает тип ракеты и боезапас\n"
                    " FIRING/f <Имя> <Наряд ракет> <Интервал пуска>\n"
                    "   Задает наряд ракет на цель и интервал пуска\n"
                    " FIRING/v <Имя> <0 или 1>\n"
                    "   Задает признак вертикального пуска\n",
                    &RSS_Module_ZRK::cFiring },
 {  NULL }
                                                            } ;

/********************************************************************/
/*                                                                  */
/*                       Общие члены класса                         */

    struct RSS_Module_ZRK_instr *RSS_Module_ZRK::mInstrList=NULL ;


/********************************************************************/
/*                                                                  */
/*                        Конструктор класса                        */

     RSS_Module_ZRK::RSS_Module_ZRK(void)

{
           keyword="EmiStand" ;
    identification="ZRK" ;
          category="Object" ;

        mInstrList=RSS_Module_ZRK_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*                                                                  */
/*                         Деструктор класса                        */

    RSS_Module_ZRK::~RSS_Module_ZRK(void)

{
}


/********************************************************************/
/*                                                                  */
/*                        Создание объекта                          */

  RSS_Object *RSS_Module_ZRK::vCreateObject(RSS_Model_data *data)

{
       RSS_Object_ZRK *object ;
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
              SEND_ERROR("Секция ZRK: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция ZRK: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция ZRK: Не задана модель объекта") ;
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
              SEND_ERROR("Секция ZRK: Неизвестная модель тела") ;
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

              SEND_ERROR("Секция ZRK: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_ZRK ;
    if(object==NULL) {
              SEND_ERROR("Секция ZRK: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }
/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция ZRK: Переполнение памяти") ;
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
                         SEND_ERROR("Секция ZRK: Переполнение памяти") ;
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
                                         data->sections[i].decl, "ZRK.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция ZRK: Переполнение памяти") ;
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

     int  RSS_Module_ZRK::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Зенитный ракетный комплекс", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                         Выполнить команду                        */

  int  RSS_Module_ZRK::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "ZRK"
#define  _SECTION_SHRT_NAME   "ZRK"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object ZRK:") ;
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
       if(status)  SEND_ERROR("Секция ZRK: Неизвестная команда") ;
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

    void  RSS_Module_ZRK::vReadSave(std::string *data)

{
                char *buff ;
                 int  buff_size ;
      RSS_Model_data  create_data ;
      RSS_Object_ZRK *object ;
                char  name[128] ;
                char *entry ;
                char *end ;
                 int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE ZRK\n", 
                      strlen("#BEGIN MODULE ZRK\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT ZRK\n", 
                      strlen("#BEGIN OBJECT ZRK\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT ZRK\n", 
              strlen("#BEGIN OBJECT ZRK\n"))) {                     /* IF.1 */
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
                object=(RSS_Object_ZRK *)vCreateObject(&create_data) ;
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
                                                 }                  /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                     Записать данные в строку                     */

    void  RSS_Module_ZRK::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE ZRK\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                    Реализация инструкции HELP                    */

  int  RSS_Module_ZRK::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_ZRK_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 Реализация инструкции CREATE                     */
/*                                                                  */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_ZRK::cCreate(char *cmd)

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
                                     Object_ZRK_Create_dialog, 
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

  int  RSS_Module_ZRK::cInfo(char *cmd)

{
                char  *name ;
      RSS_Object_ZRK  *object ;
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

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
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
                    "Owner  %s\r\n" 
                    "Target %s\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->state.x, object->state.y, object->state.z,
                        object->state.azim, object->state.elev, object->state.roll,
                        object->v_abs,
                        object->state.x_velocity, object->state.y_velocity, object->state.z_velocity,
                        object->owner, object->target
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
/*	                                                                */
/*	                  Реализация инструкции COPY                    */
/*                                                                  */
/*       COPY <Имя образца> <Имя нового объекта>                    */

  int  RSS_Module_ZRK::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *copy ;
        RSS_Object_ZRK  *sample ;
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

       sample=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект-цель по имени */
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

  int  RSS_Module_ZRK::cOwner(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *owner ;
      RSS_Object_ZRK  *e_object ;
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

       e_object=(RSS_Object_ZRK *)FindObject(name, 1) ;             /* Ищем объект-цель по имени */
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

  int  RSS_Module_ZRK::cTarget(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *target ;
      RSS_Object_ZRK  *e_object ;
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

       e_object=(RSS_Object_ZRK *)FindObject(name, 1) ;             /* Ищем объект-цель по имени */
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

  int  RSS_Module_ZRK::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
      RSS_Object_ZRK  *object ;
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

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
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

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                               object->state.y, 
                                               object->state.z ) ;

/*------------------------------------------------------ Отображение */

                      this->kernel->vShow(NULL) ;

               object->state_0=object->state ; 

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

  int  RSS_Module_ZRK::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
      RSS_Object_ZRK  *object ;
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

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
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

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->state.azim, 
                                            object->state.elev, 
                                            object->state.roll ) ;

/*------------------------------------------------------ Отображение */

                this->kernel->vShow(NULL) ;

               object->state_0=object->state ; 

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

  int  RSS_Module_ZRK::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
      RSS_Object_ZRK  *object ;
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

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
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

                       object->v_abs=sqrt(object->state.x_velocity*object->state.x_velocity+
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
/*                                                                  */
/*                   Реализация инструкции TRACE                    */
/*                                                                  */
/*       TRACE <Имя> [<Длительность>]                               */

  int  RSS_Module_ZRK::cTrace(char *cmd)

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
      RSS_Object_ZRK *object ;
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

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------- Контроль носителя и цели */

  if(object->owner[0]!=0) {
       object->o_owner=FindObject(object->owner, 0) ;               /* Ищем носитель по имени */
    if(object->o_owner==NULL)  return(-1) ;
                          }

  if(object->target[0]!=0) {
       object->o_target=FindObject(object->target, 0) ;             /* Ищем цель по имени */
    if(object->o_target==NULL)  return(-1) ;
                           }
/*------------------------------ Привязка стартовой точки к носителю */

  if(object->owner[0]!=0) {

       object->state=object->o_owner->state ;

                          }
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
               object->state_0=object->state ; 

         object->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->iSaveTracePoint("ADD") ;
/*- - - - - - - - - - - - - - - - - - - - - - Отображение траектории */
         object->iShowTrace_() ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Отображение объекта */
   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                               object->state.y, 
                                               object->state.z ) ;
     object->Features[i]->vBodyAngles   (NULL, object->state.azim, 
                                               object->state.elev, 
                                               object->state.roll ) ;
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
/*                    Реализация инструкции FIRING                  */
/*                                                                  */
/*       FIRING/R <Имя> <R-min> <R-max> <H-max>                     */
/*       FIRING/B <Имя> <Targets type>                              */
/*       FIRING/M <Имя> <Missile template> <Missiles cnt>           */
/*       FIRING/F <Имя> <Missiles per target> <Firing interval>     */
/*       FIRING/V <Имя> <Vertical launch flag>                      */

  int  RSS_Module_ZRK::cFiring(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
      RSS_Object_ZRK  *object ;
          RSS_Object  *missile ;
                 int   r_flag, b_flag, m_flag, f_flag, v_flag ;
                char  *end ;
              double   value_d_1, value_d_2, value_d_3 ;
                 int   value_i ;
                char  *error ;
             INT_PTR   status ;
                 int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      r_flag=0 ;
                      b_flag=0 ;
                      m_flag=0 ;
                      f_flag=0 ;
                      v_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )  r_flag=1 ;
           else if(strchr(cmd, 'b')!=NULL ||
                   strchr(cmd, 'B')!=NULL   )  b_flag=1 ;
           else if(strchr(cmd, 'm')!=NULL ||
                   strchr(cmd, 'M')!=NULL   )  m_flag=1 ;
           else if(strchr(cmd, 'f')!=NULL ||
                   strchr(cmd, 'F')!=NULL   )  f_flag=1 ;
           else if(strchr(cmd, 'v')!=NULL ||
                   strchr(cmd, 'V')!=NULL   )  v_flag=1 ;

                           cmd=end+1 ;
                        }

       if(r_flag+b_flag+m_flag+f_flag+v_flag>1) {
              SEND_ERROR("Должен быть только один из ключей: R, B, F, M или V. \n"
                         "Например: FIRING/F <Имя_объекта> ...") ;
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
                                 "Например: FIRING <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_ZRK *)FindObject(name, 1) ;               /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------- Задание через диалог */

    if(r_flag+b_flag+m_flag+f_flag+v_flag==0) {                     /* Если не задано ни одного флага... */

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_FIRING", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_ZRK_Firing_dialog, 
                                      (LPARAM)object               ) ;
           return((int)status) ;
                                              }
/*--------------------------------------------------- Пропись данных */

    if(r_flag) {

      if(pars[3]==NULL) {
                          SEND_ERROR("Должно быть задано 3 параметра: FIRING/R <Имя> <R-min> <R-max> <H-max>") ;
                                             return(-1) ;
                        }

         value_d_1=strtod(pars[1], &end) ;
         value_d_2=strtod(pars[2], &end) ;
         value_d_3=strtod(pars[3], &end) ;

                                error=  NULL ;
      if(value_d_1<   0.     )  error="Параметр R-min не может быть меньше 0" ;
      if(value_d_2<=  0.     )  error="Параметр R-mах должен быть больше 0" ;
      if(value_d_3<=  0.     )  error="Параметр H-mах должен быть больше 0" ;
//    if(value_d_3> value_d_2)  error="Параметр H-mах не может быть больше R-max" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                  object->r_firing_min=value_d_1 ;
                  object->r_firing_max=value_d_2 ;
                  object->h_firing_max=value_d_3 ;
               }
    else 
    if(b_flag) {

      if(pars[1]==NULL) {
                          SEND_ERROR("Должен быть задан параметра: FIRING/B <Имя> <Тип цели>") ;
                                             return(-1) ;
                        }

         value_i=strtoul(pars[1], &end, 10) ;

                         error=  NULL ;
      if(value_i!=0 &&
         value_i!=1   )  error="Значение параметра должно быть 0 или 1" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                        object->ballistic_target=value_i ;

               }
    else 
    if(m_flag) {

      if(pars[2]==NULL) {
                          SEND_ERROR("Должно быть задано 2 параметра: FIRING/M <Имя> <Missile template> <Missiles cnt>") ;
                                             return(-1) ;
                        }

         missile=FindObject(pars[1],  0) ;                          /* Ищем ракету по имени */
         value_i=   strtoul(pars[2], &end, 10) ;

                         error=  NULL ;
      if(missile==NULL)  error="неизвестная ракета" ;
      if(value_i <  0 )  error="Число ракет не может быть меньше 0" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                strncpy(object->missile_type, pars[1], sizeof(object->missile_type)-1) ;
                        object->missile_tmpl=missile ;
                        object->missiles_max=value_i ;
               }
    else 
    if(f_flag) {

      if(pars[2]==NULL) {
                          SEND_ERROR("Должно быть задано 2 параметра: FIRING/F <Имя> <Missiles per target> <Firing interval>") ;
                                             return(-1) ;
                        }

         value_i  =strtoul(pars[1], &end, 10) ;
         value_d_2= strtod(pars[2], &end) ;

                         error=  NULL ;
      if(value_i  < 1 )  error="Число ракет в залпе не может быть меньше 1" ;
      if(value_d_2<=0.)  error="Интервал пуска должен быть больше 0" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                        object->missiles_firing=value_i ;
                        object->firing_pause   =value_d_2 ;

               }
    else 
    if(v_flag) {

      if(pars[1]==NULL) {
                          SEND_ERROR("Должен быть задан параметра: FIRING/V <Имя> <Vertical launch flag>") ;
                                             return(-1) ;
                        }

         value_i=strtoul(pars[1], &end, 10) ;

                         error=  NULL ;
      if(value_i!=0 &&
         value_i!=1   )  error="Значение параметра должно быть 0 или 1" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                        object->v_launch=value_i ;

               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                Поиск обьекта типа ZRK по имени                   */

  RSS_Object *RSS_Module_ZRK::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "ZRK")) {

           SEND_ERROR("Объект не является объектом типа ZRK") ;
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
/**     ОПИСАНИЕ КЛАССА ОБЪЕКТА "ЗЕНИТНЫЙ РАКЕТНЫЙ КОМПЛЕКС"       **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*                                                                  */
/*                   Статические переменные                         */

/********************************************************************/
/*                                                                  */
/*                       Конструктор класса                         */

     RSS_Object_ZRK::RSS_Object_ZRK(void)

{
   strcpy(Type, "ZRK") ;

   CalculateExt_use= 0 ;

    Context        =new RSS_Transit_ZRK ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     battle_state= 0 ;

           v_abs = 0. ;

             start_flag=0 ;

           r_firing_min   =0. ;
           r_firing_max   =0. ;
           h_firing_max   =0. ;
          ballistic_target=0 ;
           missiles_max   =1 ;
           missiles_firing=1 ;
             firing_pause =0 ;
                  v_launch=0 ;

    strcpy(missile_type, "") ;

      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=RGB(0, 0, 127) ;
      mTrace_dlist=  0 ;  
}


/********************************************************************/
/*                                                                  */
/*                          Деструктор класса                       */

    RSS_Object_ZRK::~RSS_Object_ZRK(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*                                                                  */
/*                     Освобождение ресурсов                        */

  void   RSS_Object_ZRK::vFree(void)

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

    void  RSS_Object_ZRK::vPush(void)

{
     state_stack=state ;   
}


    void  RSS_Object_ZRK::vPop(void)

{
     state=state_stack ;   

  if(this->mTrace!=NULL) {
                             free(this->mTrace) ;
                                  this->mTrace    =NULL ;
                                  this->mTrace_cnt=  0 ;  
                                  this->mTrace_max=  0 ;  

                                      iShowTrace_() ;
                         }
}


/********************************************************************/
/*                                                                  */
/*                         Копировать объект                        */

    class RSS_Object *RSS_Object_ZRK::vCopy(char *name)

{
        RSS_Model_data  create_data ;
        RSS_Object_ZRK *object ;
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

       object=(RSS_Object_ZRK *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;

            strcpy(object->missile_type,    this->missile_type) ;
                   object->missile_tmpl    =this->missile_tmpl ;
                   object->r_firing_min    =this->r_firing_min ;
                   object->r_firing_max    =this->r_firing_max ;
                   object->h_firing_max    =this->h_firing_max ;
                   object->ballistic_target=this->ballistic_target ;
                   object->missiles_max    =this->missiles_max ;
                   object->missiles_firing =this->missiles_firing ;
                   object->firing_pause    =this->firing_pause ;
                   object->v_launch        =this->v_launch ;

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

    void  RSS_Object_ZRK::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT ZRK\n" ;

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
/*                                                                  */
/*                        Обработка событий                         */

     int  RSS_Object_ZRK::vEvent(char *event_name, double  t, char *callback, int cb_size)
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

     int  RSS_Object_ZRK::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*                                                                  */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_ZRK::vCalculateStart(double  t)
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

        this->state=this->o_owner->state_0 ;

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

/*------------------------------- Установка признака начального шага */

            this->missiles_cnt=this->missiles_max ;
            this->missiles_idx=  0 ;

            this->start_flag=   1 ;
            this->stage     =_SEARCH_STAGE ;

/*------------------------------------------------ Очистка контекста */

     for(i=0 ; i<_TRACES_MAX ; i++)  this->traces[i].target=NULL ;

            this->iSaveTracePoint("CLEAR") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_ZRK::vCalculate(double t1, double t2, char *callback, int cb_size)
{
   RSS_Object *target ;
   RSS_Object *missile ;
         char  prefix[128] ;
         char  command[1024] ;
       double  dx, dy, dz ;
       double  a, b, c ;
       double  tx_1, tx_2 ;
       double  r ;
       double  r_m ;
       double  azim, elev ;
          int  i ;
          int  j ;

#define   OBJECTS       ProgramModule.kernel->kernel_objects 
#define   OBJECTS_CNT   ProgramModule.kernel->kernel_objects_cnt 

/*------------------------------------------------------- Подготовка */

        sprintf(prefix, "%s_", this->Name) ;

/*-------------------------------------------------------- Положение */

                   state.x += state.x_velocity*(t2-t1) ;
                   state.y += state.y_velocity*(t2-t1) ;
                   state.z += state.z_velocity*(t2-t1) ;

/*----------------------------------------- Анализ состояния объекта */

         if(this->missiles_cnt<=0)  return(0) ;                     /* Если боезапас закончился... */

/*---------------------------------------- Контроль запущенных ракет */

       for(i=0 ; i<_TRACES_MAX ; i++) {

#define  T    this->traces[i]

         if(T.target==NULL)  continue ;                             /* Если свободный слот... */

         if(T.next  >  0  )  continue ;                             /* Если залп еще не завершен... */

         if(T.missile==NULL) {                                      /* Однократно определяем ссылку на объект ЗУР     */
            for(j=OBJECTS_CNT-1 ; j>=0 ; j--)                       /*  Для быстроты ищем ЗУР с конца списка объектов */
              if(!strcmp(OBJECTS[j]->Name, T.missile_name)) {
                              T.missile=OBJECTS[j] ;
                                    break ;
                                                            }
                             }

         if(T.missile==NULL)  continue ;              /* Если ЗУР не нашлась среди объектов :-( */

            r  =sqrt((T.target->state_0.x-this->state_0.x)*(T.target->state_0.x-this->state_0.x)+
                     (T.target->state_0.y-this->state_0.y)*(T.target->state_0.y-this->state_0.y)+
                     (T.target->state_0.z-this->state_0.z)*(T.target->state_0.z-this->state_0.z) ) ;
            r_m=sqrt((T.missile->state_0.x-this->state_0.x)*(T.missile->state_0.x-this->state_0.x)+
                     (T.missile->state_0.y-this->state_0.y)*(T.missile->state_0.y-this->state_0.y)+
                     (T.missile->state_0.z-this->state_0.z)*(T.missile->state_0.z-this->state_0.z) ) ;

         if(r < r_m) {                                              /* Если последняя ракета залпа пролетела цель - */
                          T.target=NULL ;                           /*  - исключаем цель из списка обстреливаемых   */
                     }

#undef   T

                                      }
/*------------------------------------ Отслеживание интервала пусков */

    if(t1-this->last_firing < this->firing_pause)  return(0)  ;

/*---------------------------------------- Анализ целевой обстановки */

    if(this->stage==_SEARCH_STAGE) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Перебор целей */ 
      for(i=0 ; i<OBJECTS_CNT ; i++) {                              /* Перебор объектов */

         if(OBJECTS[i]==this)  continue ;                           /* Если это - сам объект... */       
/*- - - - - - - - - - - - - - - - - - - - - - Отсечение по типу цели */
         if(!stricmp(OBJECTS[i]->Type, "Marker") ||
            !stricmp(OBJECTS[i]->Type, "Body"  )   )  continue ;

         if(!memicmp(OBJECTS[i]->Name, prefix,                      /* Исключение собственных ЗУР */
                                strlen(prefix)))  continue ;
/*- - - - - - - - - - - - - - - - - - - -  Выделение воздушных целей */
         if(OBJECTS[i]->state_0.y<=0)  continue ;                   /* Если это - НЕ воздушная цель... */

        if(this->ballistic_target==1) 
         if(OBJECTS[i]->state_0.y_velocity>=0)  continue ;          /* Если это - НЕ баллистическая цель... */
/*- - - - - - - - - - - - - - - -  Отсечение целей по зоне поражения */
         if(OBJECTS[i]->state_0.y>this->h_firing_max)  continue ;      /* Если цель слишком высоко... */

            r=sqrt((OBJECTS[i]->state_0.x-this->state_0.x)*(OBJECTS[i]->state_0.x-this->state_0.x)+
                   (OBJECTS[i]->state_0.z-this->state_0.z)*(OBJECTS[i]->state_0.z-this->state_0.z) ) ;
         if(r<this->r_firing_min ||
            r>this->r_firing_max   )  continue ;                    /* Если цель вне зоны поражения... */
/*- - - - - - - - - - - - - - - - - -  Если цель уже обстреливаается */
       for(j=0 ; j<_TRACES_MAX ; j++)
         if(this->traces[j].target==OBJECTS[i])  break ;

         if(j<_TRACES_MAX)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Приоритезация целей */
                 target= OBJECTS[i] ;

            this->stage=_FIRING_FIRST_STAGE ;

                 break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Перебор целей */
                                     }                              /* Перебор объектов */
 
                                   }
/*--------------------------------------- Пуск первой ракеты в залпе */

    if(this->stage==_FIRING_FIRST_STAGE) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Регистрация цели */
       for(j=0 ; j<_TRACES_MAX ; j++)
         if(this->traces[j].target==NULL)  break ;

         if(j>=_TRACES_MAX) {                                       /* Если свободных слотов нет... */
                               this->stage=_FIRING_FIRST_STAGE ;
                                        return(0) ;
                            }

                   this->traces[j].target =target ;
           sprintf(this->traces[j].missile_name, "%s%d", prefix, this->missiles_idx) ;
                   this->traces[j].missile=NULL ;
                   this->traces[j].next   =this->missiles_firing ;
/*- - - - - - - - - - - - Расчет углов наведения в упрежденную точку */
          missile=this->missile_tmpl ;

               dx=target->state_0.x-this->state_0.x ;
               dy=target->state_0.y-this->state_0.y ;
               dz=target->state_0.z-this->state_0.z ;

                a=target ->state_0.x_velocity*target ->state_0.x_velocity
                 +target ->state_0.y_velocity*target ->state_0.y_velocity
                 +target ->state_0.z_velocity*target ->state_0.z_velocity
                 -missile->state_0.x_velocity*missile->state_0.x_velocity
                 -missile->state_0.y_velocity*missile->state_0.y_velocity
                 -missile->state_0.z_velocity*missile->state_0.z_velocity ;
                b=2.*( target->state_0.x_velocity*dx
                      +target->state_0.y_velocity*dy
                      +target->state_0.z_velocity*dz ) ;
                c=dx*dx+dy*dy+dz*dz ;

             tx_1=(-b-sqrt(b*b-4.*a*c))/(2.*a) ;
             tx_2=(-b+sqrt(b*b-4.*a*c))/(2.*a) ;

              if(tx_1<0. && tx_2<0.)  tx_1=   0. ;
         else if(tx_1<0.           )  tx_1=tx_2 ;
         else if(tx_2<0.           )   ;
         else if(tx_1>tx_2         )  tx_1=tx_2 ;

              dx+=target->state_0.x_velocity*tx_1 ;
              dy+=target->state_0.y_velocity*tx_1 ;
              dz+=target->state_0.z_velocity*tx_1 ;

               r= sqrt(dx*dx+dz*dz) ;
            azim=atan2(dx, dz)*_RAD_TO_GRD ;
            elev=atan2(dy,  r)*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - -  Формирование команд на пуск */ 
             sprintf(command, "EXEC %s COPY %s;"
                              "EXEC %s TARGET %s;"
                              "EXEC %s BASE %.0lf %.0lf %.0lf;"
                              "START %s;",
                               this->missile_type, this->traces[j].missile_name,
                                                   this->traces[j].missile_name, target->Name,
                                                   this->traces[j].missile_name, this->state_0.x, this->state_0.y, this->state_0.z,
                                                   this->traces[j].missile_name ) ;
              strcat(callback, command) ;

         if(this->v_launch==0) {
             sprintf(command, "EXEC %s ANGLE %.0lf %.0lf 0;",
                                  this->traces[j].missile_name, azim, elev) ;
              strcat(callback, command) ;
                               }

            this->missiles_cnt-- ;
            this->missiles_idx++ ;
            this->last_firing   =  t1 ;
            this->firing_channel=&this->traces[j] ;

            this->firing_channel->next-- ;
         if(this->firing_channel->next>0)  this->stage=_FIRING_NEXT_STAGE ;
         else                              this->stage=     _SEARCH_STAGE ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*------------------------- Пуск второй и последующих ракеты в залпе */

    else
    if(this->stage==_FIRING_NEXT_STAGE) {

                 target=this->firing_channel->target ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Регистрация ракеты */
           sprintf(this->firing_channel->missile_name, "%s%d", prefix, this->missiles_idx) ;
/*- - - - - - - - - - - - Расчет углов наведения в упрежденную точку */
          missile=this->missile_tmpl ;

               dx=target->state_0.x-this->state_0.x ;
               dy=target->state_0.y-this->state_0.y ;
               dz=target->state_0.z-this->state_0.z ;

                a=target ->state_0.x_velocity*target ->state_0.x_velocity
                 +target ->state_0.y_velocity*target ->state_0.y_velocity
                 +target ->state_0.z_velocity*target ->state_0.z_velocity
                 -missile->state_0.x_velocity*missile->state_0.x_velocity
                 -missile->state_0.y_velocity*missile->state_0.y_velocity
                 -missile->state_0.z_velocity*missile->state_0.z_velocity ;
                b=2.*( target->state_0.x_velocity*dx
                      +target->state_0.y_velocity*dy
                      +target->state_0.z_velocity*dz ) ;
                c=dx*dx+dy*dy+dz*dz ;

             tx_1=(-b-sqrt(b*b-4.*a*c))/(2.*a) ;
             tx_2=(-b+sqrt(b*b-4.*a*c))/(2.*a) ;

              if(tx_1<0. && tx_2<0.)  tx_1=   0. ;
         else if(tx_1<0.           )  tx_1=tx_2 ;
         else if(tx_2<0.           )   ;
         else if(tx_1>tx_2         )  tx_1=tx_2 ;

              dx+=target->state_0.x_velocity*tx_1 ;
              dy+=target->state_0.y_velocity*tx_1 ;
              dz+=target->state_0.z_velocity*tx_1 ;

               r= sqrt(dx*dx+dz*dz) ;
            azim=atan2(dx, dz)*_RAD_TO_GRD ;
            elev=atan2(dy,  r)*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - -  Формирование команд на пуск */ 
             sprintf(command, "EXEC %s COPY %s;"
                              "EXEC %s TARGET %s;"
                              "EXEC %s BASE %.0lf %.0lf %.0lf;"
                              "START %s;",
                               this->missile_type, this->firing_channel->missile_name,
                                                   this->firing_channel->missile_name, target->Name,
                                                   this->firing_channel->missile_name, this->state_0.x, this->state_0.y, this->state_0.z,
                                                   this->firing_channel->missile_name ) ;
              strcat(callback, command) ;

         if(this->v_launch==0) {
             sprintf(command, "EXEC %s ANGLE %.0lf %.0lf 0;",
                                  this->firing_channel->missile_name, azim, elev) ;
              strcat(callback, command) ;
                               }

            this->missiles_cnt-- ;
            this->missiles_idx++ ;
            this->last_firing = t1 ;

            this->firing_channel->next-- ;
         if(this->firing_channel->next>0)  this->stage=_FIRING_NEXT_STAGE ;
         else                              this->stage=     _SEARCH_STAGE ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                        }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_ZRK::vCalculateShow(double  t1, double  t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* Сохранение точки траектории */  

         this->iShowTrace_() ;                                      /* Отображение траектории */

   for(i=0 ; i<this->Features_cnt ; i++) {                          /* Отображение объекта */
     this->Features[i]->vBodyBasePoint(NULL, this->state.x, 
                                             this->state.y, 
                                             this->state.z ) ;
     this->Features[i]->vBodyAngles   (NULL, this->state.azim, 
                                             this->state.elev, 
                                             this->state.roll ) ;
                                         }

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   Сохранение точки траектории                     */

  int  RSS_Object_ZRK::iSaveTracePoint(char *action)

{
/*------------------------------------------------- Сброс траектории */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- Довыделение буфера */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_ZRKTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_ZRKTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("ZRK.iSaveTracePoint@"
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
                  mTrace[mTrace_cnt].color     =RGB(  0, 0, 127) ;
                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*           Отображение траектории с передачей контекста           */

  void  RSS_Object_ZRK::iShowTrace_(void)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Отображение траектории                        */

  void  RSS_Object_ZRK::iShowTrace(void)

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

     RSS_Transit_ZRK::RSS_Transit_ZRK(void)

{
}


/*********************************************************************/
/*                                                                   */
/*               Деструктор класса "ТРАНЗИТ КОНТЕКСТА"               */

    RSS_Transit_ZRK::~RSS_Transit_ZRK(void)

{
}


/********************************************************************/
/*                                                                  */
/*                    Исполнение действия                           */

    int  RSS_Transit_ZRK::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_ZRK *)object)->iShowTrace() ;

   return(0) ;
}

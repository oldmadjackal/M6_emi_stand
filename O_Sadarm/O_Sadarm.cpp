/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "SADARM"  		    */
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

#include "O_Sadarm.h"
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

     static   RSS_Module_Sadarm  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_SADARM_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_SADARM_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "SADARM"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Sadarm_instr  RSS_Module_Sadarm_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Sadarm::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_Sadarm::cCreate },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_Sadarm::cInfo },
 { "copy",    "cp", "#COPY - копировать объект",
                    " COPY <Имя образца> <Имя нового объекта>\n"
                    "   Копировать объект"
                    " COPY <Имя образца> <Имя нового объекта> <x> <y> <z>\n"
                    "   Копировать объект в заданную точку",
                    &RSS_Module_Sadarm::cCopy },
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
                    &RSS_Module_Sadarm::cBase },
 { "velocity", "v", "#VELOCITY - задание скорости снижения",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задание скорости снижения суббоеприпаса",
                    &RSS_Module_Sadarm::cVelocity },
 { "scan",     "s", "#SCAN - задание параметров сканирования",
                    " SCAN/A <Имя> <Высота>\n"
                    "   Задание начальной высоты сканирования"
                    " SCAN/T <Имя> <Угол>\n"
                    "   Задание угла наклона заряда к вертикали"
                    " SCAN/R <Имя> <Обороты в минуту>\n"
                    "   Задание скорости вращения сканирования"
                    " SCAN/V <Имя> <Угол>\n"
                    "   Задание угла поля зрения датчика цели",
                    &RSS_Module_Sadarm::cScan },
 { "trace",    "t", "#TRACE - трассировка траектории объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Трассировка траектории объекта в реальном времени\n",
                    &RSS_Module_Sadarm::cTrace },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Sadarm_instr *RSS_Module_Sadarm::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Sadarm::RSS_Module_Sadarm(void)

{
	   keyword="EmiStand" ;
    identification="Sadarm" ;
          category="Object" ;

        mInstrList=RSS_Module_Sadarm_InstrList ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Sadarm::~RSS_Module_Sadarm(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Sadarm::vCreateObject(RSS_Model_data *data)

{
        RSS_Object_Sadarm *object ;
   RSS_Feature_Hit_Sadarm *hit ;
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
              SEND_ERROR("Секция SADARM: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция SADARM: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция SADARM: Не задана модель объекта") ;
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
              SEND_ERROR("Секция SADARM: Неизвестная модель тела") ;
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

              SEND_ERROR("Секция SADARM: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_Sadarm ;
    if(object==NULL) {
              SEND_ERROR("Секция SADARM: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }
/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция SADARM: Переполнение памяти") ;
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
                         SEND_ERROR("Секция SADARM: Переполнение памяти") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;

/*---------------------------------- Создание списка свойств обьекта */

      object->Features_cnt=this->feature_modules_cnt ;
      object->Features    =(RSS_Feature **)
                             calloc(this->feature_modules_cnt, 
                                     sizeof(object->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++) 
     if(!stricmp(this->feature_modules[i]->identification, "Hit")) {
                          hit =new RSS_Feature_Hit_Sadarm ;
           object->Features[i]=this->feature_modules[i]->vCreateFeature(object, hit) ;
                                                                   }
     else                                                          {
           object->Features[i]=this->feature_modules[i]->vCreateFeature(object, NULL) ;
                                                                   }

/*-------------------------------------- Считывание описаний свойств */

           RSS_Model_ReadSect(data) ;                               /* Считываем секции описаний модели */

   for(i=0 ; data->sections[i].title[0] ; i++) {

     for(j=0 ; j<object->Features_cnt ; j++) {

          object->Features[j]->vBodyPars(NULL, PAR) ;
          object->Features[j]->vReadSave(data->sections[i].title, 
                                         data->sections[i].decl, "Sadarm.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция SADARM: Переполнение памяти") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

       strcpy(object->Name,       data->name) ;
       strcpy(object->model_path, data->path) ;

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

     int  RSS_Module_Sadarm::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Суббоеприпас типа SADARM", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_Sadarm::vShow(char *layer)

{
     int  status ;
     int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
  
/*--------------------------------------------- Отображение эффектов */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - Перебор контекстов отображения */

     for(i=0 ; i<OBJECTS_CNT ; i++) {

       if(stricmp(OBJECTS[i]->Type, "Sadarm"))  continue ;

           ((RSS_Object_Sadarm *)OBJECTS[i])->iShowView() ;
                                    }

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - Перебор контекстов отображения */
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Sadarm::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "SADARM"
#define  _SECTION_SHRT_NAME   "SADARM"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Sadarm:") ;
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
       if(status)  SEND_ERROR("Секция SADARM: Неизвестная команда") ;
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

    void  RSS_Module_Sadarm::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
  RSS_Object_Sadarm *object ;
               char  name[128] ;
               char *entry ;
               char *end ;
                int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE SADARM\n", 
                      strlen("#BEGIN MODULE SADARM\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT SADARM\n", 
                      strlen("#BEGIN OBJECT SADARM\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT SADARM\n", 
              strlen("#BEGIN OBJECT SADARM\n"))) {                  /* IF.1 */
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
                object=(RSS_Object_Sadarm *)vCreateObject(&create_data) ;
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

    void  RSS_Module_Sadarm::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE SADARM\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Sadarm::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Sadarm_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_Sadarm::cCreate(char *cmd)

{
 RSS_Model_data  data ;
     RSS_Object *object ;
           char *name ;
           char *model ;
           char *pars[10] ;
           char *end ;
           char  tmp[1024] ;
            int  status ;
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
                                     Object_Sadarm_Create_dialog, 
                                    (LPARAM)&data               ) ;
   if(status)  return(status) ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Sadarm::cInfo(char *cmd)

{
               char  *name ;
  RSS_Object_Sadarm  *object ;
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

       object=(RSS_Object_Sadarm *)FindObject(name, 1) ;           /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 8.2lf\r\n" 
                    "     Y % 8.2lf\r\n" 
                    "     Z % 8.2lf\r\n"
                    "Vel. V % 8.2lf\r\n" 
                    "Altit. % 8.2lf\r\n" 
                    "Tilt   % 8.2lf\r\n" 
                    "Rotat. % 8.2lf\r\n" 
                    "View   % 8.2lf\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->v_abs, object->scan_altitude, 
                        object->scan_tilt, object->scan_rotation, object->scan_view
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
/*       COPY <Имя образца> <Имя нового объекта> <X> <Y> <Z>        */

  int  RSS_Module_Sadarm::cCopy(char *cmd)

{
#define   _PARS_MAX  10
                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *copy ;
     RSS_Object_Sadarm  *sadarm ;
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

       sadarm=(RSS_Object_Sadarm *)FindObject(name, 1) ;            /* Ищем объект-цель по имени */
    if(sadarm==NULL)  return(-1) ;

/*------------------------------------------ Контроль имени носителя */

    if(copy==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта-копии. \n"
                                 "Например: COPY <Имя образца> <Имя нового объекта>") ;
                                     return(-1) ;
                   }
/*---------------------------------------------- Копирование объекта */

       object=sadarm->vCopy(copy) ;
    if(object==NULL)  return(-1) ;

/*------------------------------------------ Установка базовой точки */

    if(pars[2]!=NULL)  object->x_base=strtod(pars[2], &end) ;
    if(pars[3]!=NULL)  object->y_base=strtod(pars[3], &end) ;
    if(pars[4]!=NULL)  object->z_base=strtod(pars[4], &end) ;

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                               object->y_base, 
                                               object->z_base ) ;

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

  int  RSS_Module_Sadarm::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
  RSS_Object_Sadarm  *object ;
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

       object=(RSS_Object_Sadarm *)FindObject(name, 1) ;            /* Ищем объект по имени */
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
/*		      Реализация инструкции VELOCITY                */
/*								    */
/*       VELOCITY <Имя> <Скорость>                                  */

  int  RSS_Module_Sadarm::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
  RSS_Object_Sadarm  *object ;
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

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: VELOCITY <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Sadarm *)FindObject(name, 1) ;            /* Ищем объект по имени */
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
      if(coord_cnt==0)  error="Не указана скорость" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись скорости */

                          object->v_abs=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SCAN                    */
/*								    */
/*   SCAN/A <Имя> <Высота>  Задание начальной высоты сканирования   */
/*   SCAN/T <Имя> <Угол>    Задание угла наклона заряда к вертикали */
/*   SCAN/R <Имя> <Обороты в минуту>                                */
/*                          Задание скорости вращения сканирования" */
/*   SCAN/V <Имя> <Угол>    Задание угла поля зрения датчика цели"  */

  int  RSS_Module_Sadarm::cScan(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
       RSS_Object_Sadarm  *object ;
                     int   a_flag ;            /* Флаг начальной высоты сканирования */
                     int   t_flag ;            /* Флаг угла наклона */
                     int   r_flag ;            /* Флаг скорости вращения */
                     int   v_flag ;            /* Флаг полязрения */
                    char   *error ;
                    char   *end ;
                     int    i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          a_flag=0 ;
                          t_flag=0 ;
                          r_flag=0 ;
                          v_flag=0 ;

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
                if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )  r_flag=1 ;
                if(strchr(cmd, 'v')!=NULL ||
                   strchr(cmd, 'V')!=NULL   )  v_flag=1 ;

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

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: SCAN <Имя_объекта> ...") ;
                                         return(-1) ;
                         }

       object=(RSS_Object_Sadarm *)FindObject(name, 1) ;            /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------- Контроль вида срабатывания */

    if(a_flag+
       t_flag+
       r_flag+
       v_flag==0   ) {
                        SEND_ERROR("Не задан вид параметра сканирования. \n"
                                   "Необходимо использовать SCAN/A, SCAN/T, SCAN/R или SCAN/V") ;
                                         return(-1) ;
                     }

    if(a_flag+
       t_flag+
       r_flag+
       v_flag>1    ) {
                        SEND_ERROR("Допустимо задание только одного параметра сканирования. \n"
                                   "Необходимо использовать SCAN/A, SCAN/T, SCAN/R или SCAN/V") ;
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
      if(coord_cnt==0)  error="Не указан параметр сканирования" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись значений */

   if(a_flag) object->scan_altitude=coord[0] ;
   if(t_flag) object->scan_tilt    =coord[0] ;
   if(r_flag) object->scan_rotation=coord[0] ;
   if(v_flag) object->scan_view    =coord[0] ;

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

  int  RSS_Module_Sadarm::cTrace(char *cmd)

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
  RSS_Object_Sadarm *object ;
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

       object=(RSS_Object_Sadarm *)FindObject(name, 1) ;            /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------ Привязка стартовой точки к носителю */


/*------------------------------------------------------ Трассировка */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

    do {                                                            /* CIRCLE.1 - Цикл трассировки */
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отработка времени */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

           if(time_1-time_0>trace_time)  break ;                    /* Если время трассировки закончилось */

              time_w=time_c-(time_1-time_0) ;

           if(time_w>=0)  Sleep(time_w*1000) ;
/*- - - - - - - - - - - - - - - - - - - - - - Моделирование движения */
         object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->vCalculateShow() ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка сцены */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
           if(object->y_base<=0.)  break ;

       } while(1) ;                                                 /* END CIRCLE.1 - Цикл трассировки */

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа SADARM по имени               */

  RSS_Object *RSS_Module_Sadarm::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Sadarm")) {

           SEND_ERROR("Объект не является объектом типа SADARM") ;
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
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "SADARM"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_Sadarm::RSS_Object_Sadarm(void)

{
   strcpy(Type, "Sadarm") ;

          Module=&ProgramModule ;

    Context        =new RSS_Transit_Sadarm ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

 battle_state   =  0 ; 

      x_base    =  0. ;
      y_base    =  0. ;
      z_base    =  0. ;
      a_azim    =  0. ;
      a_elev    =  0. ;
      a_roll    =  0. ;
   scan_altitude=200. ;
   scan_tilt    = 30. ;
   scan_rotation= 10. ;
   scan_view    = 10. ;
      v_abs     =  5. ;

    hit_flag    =  0 ;
   view_dlist   =  0 ;

}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Sadarm::~RSS_Object_Sadarm(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Object_Sadarm::vFree(void)

{
  int  i ;


  if(this->Features!=NULL) {

    for(i=0 ; i<this->Features_cnt ; i++) {
                this->Features[i]->vBodyDelete(NULL) ;
           free(this->Features[i]) ;
                                         }

           free(this->Features) ;

                this->Features    =NULL ;
                this->Features_cnt=  0 ;
                           }

      if(this->view_dlist)
           RSS_Kernel::display.ReleaseList(this->view_dlist) ;

}


/********************************************************************/
/*								    */
/*                        Копировать объект		            */

    class RSS_Object *RSS_Object_Sadarm::vCopy(char *name)

{
        RSS_Model_data  create_data ;
     RSS_Object_Sadarm *object ;
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

       object=(RSS_Object_Sadarm *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;

       object->v_abs        =this->v_abs ;
       object->scan_altitude=this->scan_altitude ;
       object->scan_tilt    =this->scan_tilt ;
       object->scan_rotation=this->scan_rotation ;
       object->scan_view    =this->scan_view ;

   if(RSS_Kernel::battle)  object->battle_state=_SPAWN_STATE ;

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

/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_Sadarm::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT SADARM\n" ;

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

     int  RSS_Object_Sadarm::vEvent(char *event_name, double  t)
{
/*--------------------------------------------------- Поражение цели */

   if(!stricmp(event_name, "HIT")) {
                                         return(1) ;
                                   }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Object_Sadarm::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_Sadarm::vCalculateStart(double  t)
{
  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_Sadarm::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Feature_Hit *hit ;
           double  dy, da ;
             char  cb_command[1024] ;
              int  m ;

/*------------------------------------------------------- Подготовка */

    for(m=0 ; m<this->Features_cnt ; m++)                           /* Ищем свойство Hit */
      if(!strcmp(this->Features[m]->Type, "Hit"))  break ;

                    hit=(RSS_Feature_Hit *)this->Features[m] ;

/*------------------------------------------- Отработка срабатывания */

   if(this->hit_flag) {

     if(this->hit_flag==1) {
                       sprintf(cb_command,
                               "EXEC BLAST CR blast_%s %lf %lf %lf %lf %lf;"
                               "START blast_%s;",
                                this->Name, 0., 7., this->hit_point.x, this->hit_point.y, this->hit_point.z,
                                this->Name
                              ) ;
                       strncat(callback, cb_command, cb_size-1) ;
                           }

        this->hit_flag++ ;
     if(this->hit_flag>=4) {
                       sprintf(cb_command, "KILL %s;", this->Name) ;
                       strncat(callback, cb_command, cb_size-1) ;

                                  return(1) ;
                           }               

                                  return(0) ;
                      }
/*--------------------------------------- Расчет изменения положения */

               dy=-this->v_abs*(t2-t1) ;
               da= this->scan_rotation*6.*(t2-t1) ;

/*------------------ Контроль прохождения высоты начала сканирования */
       
   if(this->y_base+dy<this->scan_altitude) {
                                               hit->any_weapon=1 ;
                                           }
/*------------------------------------ Контроль прохождения 0-уровня */
       
   if(this->y_base+dy<0) {
                                         dy=-this->y_base ;
                            hit->any_weapon=  0 ;

                       sprintf(cb_command,
                               "STOP %s;",
                                this->Name
                              ) ;
                       strncat(callback, cb_command, cb_size-1) ;

                         }
/*------------------------------------------ Расчет нового положения */

          this->y_base+=dy ;
          this->a_azim+=da ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_Sadarm::vCalculateShow(void)
{
   int i ;


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


/********************************************************************/
/*								    */
/*           Отображение эффекта с передачей контекста              */

  int  RSS_Object_Sadarm::iShowView_(void)

{
       strcpy(Context->action, "SHOW_VIEW") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Отображение эффекта                           */

  int  RSS_Object_Sadarm::iShowView(void)

{
   double  r ;
   double  x_c, y_c, z_c ;
      int  status ;

/*------------------------------------------------- Входной контроль */

     if(this->y_base> this->scan_altitude ||
        this->y_base<=  0                   ) {
                                                   return(0) ;
                                              }
/*------------------------------------- Обработка дисплейных списков */

     if(view_dlist==0)  view_dlist=RSS_Kernel::display.GetList(2) ;

     if(view_dlist==0)  return(-1) ;

/*----------------------------------------------- Расчет поля зрения */

           r=this->y_base  *tan(this->scan_tilt*_GRD_TO_RAD) ;
         x_c=this->x_base+r*sin(this->a_azim*_GRD_TO_RAD) ;
         y_c= 0. ; 
         z_c=this->z_base+r*cos(this->a_azim*_GRD_TO_RAD) ;
           r=this->y_base/cos(this->scan_tilt*_GRD_TO_RAD)*sin(this->scan_view*0.5*_GRD_TO_RAD) ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------- Формирование воронки поля зрения и выстрела */

             glNewList(view_dlist, GL_COMPILE) ;                    /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;
              glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
/*- - - - - - - - - - - - - - - - - Формирование воронки поля зрения */
                               color=RGB(0, 0, 128) ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 0.2) ;

//                     glBegin(GL_POLYGON) ;
                       glBegin(GL_TRIANGLE_FAN) ;
                    glVertex3d(this->x_base, this->y_base, this->z_base) ;
                    glVertex3d( x_c+0.462*r,       y_c,     z_c+0.191*r) ;
                    glVertex3d( x_c+0.191*r,       y_c,     z_c+0.462*r) ;
                    glVertex3d( x_c-0.191*r,       y_c,     z_c+0.462*r) ;
                    glVertex3d( x_c-0.462*r,       y_c,     z_c+0.191*r) ;
                    glVertex3d( x_c-0.462*r,       y_c,     z_c-0.191*r) ;
                    glVertex3d( x_c-0.191*r,       y_c,     z_c-0.462*r) ;
                    glVertex3d( x_c+0.191*r,       y_c,     z_c-0.462*r) ;
                    glVertex3d( x_c+0.462*r,       y_c,     z_c-0.191*r) ;
                    glVertex3d( x_c+0.462*r,       y_c,     z_c+0.191*r) ;

	                 glEnd();
/*- - - - - - - - - - - - - - - - - - - - - -  Формирование выстрела */
       if(this->hit_flag) {
                               color=RGB(128, 0, 0) ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 1.0) ;

                       glBegin(GL_LINES) ;
                    glVertex3d(this->x_base,      this->y_base,      this->z_base     ) ;
                    glVertex3d(this->hit_point.x, this->hit_point.y, this->hit_point.z) ;
	                 glEnd();

                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit_Sadarm::RSS_Transit_Sadarm(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Sadarm::~RSS_Transit_Sadarm(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Sadarm::vExecute(void)

{
   if(!stricmp(action, "SHOW_VIEW"))  ((RSS_Object_Sadarm *)object)->iShowView() ;

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**           ОПИСАНИЕ КЛАССА ПОРАЖАЮЩИХ СВОЙСТВ ОБЪЕКТА           **/
/**							           **/
/********************************************************************/
/********************************************************************/


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Feature_Hit_Sadarm::RSS_Feature_Hit_Sadarm(void)

{
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Feature_Hit_Sadarm::~RSS_Feature_Hit_Sadarm(void)

{
}


/********************************************************************/
/*								    */
/*                Проверка непротиворечивости свойства              */  

    int RSS_Feature_Hit_Sadarm::vCheck(void *data, RSS_Objects_List *checked)

{
 RSS_Object_Sadarm *sadarm ;
        RSS_Object *object ;
            double  dh, da ;
            double  h, a, r ;
            double  x[12], z[12] ;
               int  n ;            /* Индекс объекта в списке объектов*/
               int  m ;            /* Индекс свойства "Пересечение" в списке свойств */
               int  hit_flag ;
               int  i ;
               int  j ;

#define  OBJECTS_CNT                     RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                         RSS_Kernel::kernel_objects
#define  HIT         ((RSS_Feature_Hit *)object->Features[m])

/*------------------------------------------------ Входной контроль  */

      sadarm=(RSS_Object_Sadarm *)this->Object ;

   if(sadarm->hit_flag)  return(0) ;                                /* Если уже сработало... */

   if(this->any_weapon==0)  return(0) ;                             /* Если цель не назначена... */

   if(this->track_s_prv.mark==0)  return(0)  ;                      /* Если стартовая точка трассировки... */

/*--------------------------------------- Определение дуги поражения */

             dh=(this->track_s_prv.y   -this->track_s.y   )/10. ;
             da=(this->track_s_prv.azim-this->track_s.azim)/10. ;

     for(j=0 ; j<11 ; j++) {

             h   =this->track_s_prv.y   +dh*j ;
             a   =this->track_s_prv.azim+da*j ;
             r   =h*tan(sadarm->scan_tilt*_GRD_TO_RAD) ;
             x[j]=sadarm->x_base+r*sin(a*_GRD_TO_RAD) ;
             z[j]=sadarm->z_base+r*cos(a*_GRD_TO_RAD) ;
                           }
/*------------------------------------------------- Обсчет поражения */

                                      hit_flag=0 ;

   for(n=0 ; n<OBJECTS_CNT ; n++) {                                 /* CIRCLE.1 - Перебор объектов */

           object=OBJECTS[n] ;

    for(m=0 ; m<object->Features_cnt ; m++)                         /* Проверяем, связано ли с данным объектом         */
      if(!strcmp(object->Features[m]->Type, this->Type ))  break ;  /*  свойство "Пересечение", если нет - обходим его */

      if(m==object->Features_cnt)  continue ;                       /* Если объект-цель не обладает своиством Hit... */

     if(HIT->track_s_prv.mark==0)  continue  ;                      /* Если стартовая точка трассировки для цели... */

     if(HIT->Bodies_cnt==0)  continue ;                             /* Если свойство Hit - пустое */

     if(this->target[0]==0)                                         /* Если пересечение с произвольной целью и это не цель */
      if(HIT->any_target==0)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - Контроль поражения тел */
     for(i=0 ; i< HIT->Bodies_cnt ; i++) {                          /* CIRCLE.2 - Перебор тел цели  */

       if(!iBodyTest(&HIT->Bodies[i], x, z, 10))  continue;     

         sadarm->hit_flag   =1 ; 
         sadarm->hit_point.x=object->x_base ; 
         sadarm->hit_point.y=object->y_base ; 
         sadarm->hit_point.z=object->z_base ; 

         checked->Add(object, this->Type) ;                         /* Регистрируем CHECK-связь */

       if(!HIT->hit_done)                                           /* Модифицируем общий счётчик поражения */
        if(hit_count!=NULL) (*hit_count)++ ;

                          HIT->hit_done=1 ; 

                       hit_flag=1 ;
                          break ;
                                         }                          /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         if(hit_flag)  break ;
                                  }                                 /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef   HIT

   return(hit_flag) ;
}


/********************************************************************/
/*								    */
/*                Контроль пересечения тела с трассой               */  

    int RSS_Feature_Hit_Sadarm::iBodyTest(RSS_Feature_Hit_Body *body,
                                            double *x, double *z, int  segments)
{
  double  A, C, B, B_prv ;
  double  E, F, F1, F2 ;
  double  dx, dz ;
     int  p1, p2 ;
     int  x_base_flag ;
     int  i ;
     int  k ;
     int  n ;

/*------------------------------------------------ Перебор ребер тел */

    for(n=0 ; n<body->Facets_cnt ; n++)
    for(k=0 ; k<body->Facets[n].vertexes_cnt ; k++) {

/*------------------------------------------ Расчет параметров ребра */

      if(k==0)  p1=body->Facets[n].vertexes[body->Facets[n].vertexes_cnt-1] ;
      else      p1=body->Facets[n].vertexes[k-1] ; 
                p2=body->Facets[n].vertexes[k  ] ;

            dx=body->Vertexes[p2].x_abs-body->Vertexes[p1].x_abs ;
            dz=body->Vertexes[p2].z_abs-body->Vertexes[p1].z_abs ;

      if(dx==0. && dz==0.)  continue ;

                             x_base_flag=0 ;
      if(fabs(dx)>fabs(dz))  x_base_flag=1 ;

      if(x_base_flag) {
                          A=dz/dx ;
                          C=body->Vertexes[p1].z_abs-A*body->Vertexes[p1].x_abs ;
                      }
      else            {
                          A=dx/dz ;
                          C=body->Vertexes[p1].x_abs-A*body->Vertexes[p1].z_abs ;
                      }
/*--------------------------- Проверка пересечения с дугой поражения */

                         B=0. ;

      for(i=0 ; i<=segments ; i++) {
/*- - - - - - - - - - - - - - - Проверка кросса ребра сегментом дуги */
                         B_prv=B ;
        if(x_base_flag)  B    =z[i]-A*x[i]-C ;
        else             B    =x[i]-A*z[i]-C ;

        if(i==0)  continue ;

        if(B*B_prv>0.)  continue ;
/*- - - - - - - - - - - - - - - Проверка кросса сегмента дуги ребром */
            dx=x[i]-x[i-1] ;
            dz=z[i]-z[i-1] ;

        if(fabs(dx)>fabs(dz)) {
                                 E =dz/dx ;
                                 F =z[i]-E*x[i] ;
                                 F1=body->Vertexes[p1].z_abs-E*body->Vertexes[p1].x_abs-F ;
                                 F2=body->Vertexes[p2].z_abs-E*body->Vertexes[p2].x_abs-F ;
                              }
        else                  {
                                 E =dx/dz ;
                                 F =x[i]-E*z[i] ;
                                 F1=body->Vertexes[p1].x_abs-E*body->Vertexes[p1].z_abs-F ;
                                 F2=body->Vertexes[p2].x_abs-E*body->Vertexes[p2].z_abs-F ;
                              }

        if(F1*F2<=0.)  return(1) ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*------------------------------------------------ Перебор ребер тел */
                                                    }
/*-------------------------------------------------------------------*/

  return(0) ;
}


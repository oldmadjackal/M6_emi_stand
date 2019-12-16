/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "РАКЕТА-ЛЕГО"  	    */
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
#include "..\Ud_tools\UserDlg.h"

#include "O_Missile_Lego.h"

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

     static   RSS_Module_MissileLego  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_MISSILE_LEGO_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_MISSILE_LEGO_API RSS_Kernel *GetEntry(void)

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

  struct RSS_Module_MissileLego_instr  RSS_Module_MissileLego_InstrList[]={

 { "help",     "?", "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_MissileLego::cHelp   },
 { "create",  "cr", "#CREATE - создать объект",
                    " CREATE <Имя> [<Модель> [<Список параметров>]]\n"
                    "   Создает именованный обьект по параметризованной модели",
                    &RSS_Module_MissileLego::cCreate },
 { "info",     "i", "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную нформацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_MissileLego::cInfo },
 { "copy",    "cp", "#COPY - копировать объект",
                    " COPY <Имя образца> <Имя нового объекта>\n"
                    "   Копировать объект",
                    &RSS_Module_MissileLego::cCopy },
 { "owner",    "o", "#OWNER - назначить носитель ракеты",
                    " OWNER <Имя> <Носитель>\n"
                    "   Назначить объект - носитель ракеты",
                    &RSS_Module_MissileLego::cOwner },
 { "target",  "tg", "#TARGET - назначить цель ракеты",
                    " TARGET <Имя> <Цель>\n"
                    "   Назначить объект - цель ракеты",
                    &RSS_Module_MissileLego::cTarget },
 { "lego",     "l", "#LEGO - назначить компонент в составе объекта",
                    " LEGO <Имя>\n"
                    "   просмотреть список компонентов, назначить компонент в диалоговом режиме\n"
                    " LEGO <Имя объекта> <Имя компонента> <Тип компонента>\n"
                    "   добавить компонент в состав объекта",
                    &RSS_Module_MissileLego::cLego },
 { "trace",    "t", "#TRACE - трассировка траектории объекта",
                    " TRACE <Имя> [<Длительность>]\n"
                    "   Трассировка траектории объекта в реальном времени\n",
                    &RSS_Module_MissileLego::cTrace },
 { "spawn",   "sp", "#SPAWN - залповый пуск с использованием шаблона объекта",
                    " SPAWN <Имя> <Число пусков> <Периодичность пусков>\n"
                    "   Залповый пуск с использованием шаблона объекта\n",
                    &RSS_Module_MissileLego::cSpawn },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_MissileLego_instr *RSS_Module_MissileLego::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_MissileLego::RSS_Module_MissileLego(void)

{
  static  WNDCLASS  View_wnd ;
              char  text[1024] ;

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="Missile-Lego" ;
          category="Object" ;

        mInstrList=RSS_Module_MissileLego_InstrList ;

/*--------------------------- Регистрация класса окна UD_Show_view2D */

  if(View_wnd.hInstance==NULL) {

	View_wnd.lpszClassName="O_MissileLego_view_class" ;
	View_wnd.hInstance    = GetModuleHandle(NULL) ;
	View_wnd.lpfnWndProc  = UD_diagram_2D_prc ;
	View_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	View_wnd.hIcon        =  NULL ;
	View_wnd.lpszMenuName =  NULL ;
	View_wnd.hbrBackground=  NULL ;
	View_wnd.style        =    0 ;
	View_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&View_wnd)) {
              sprintf(text, "O_MissileLego_view_class register error %d", GetLastError()) ;
           SEND_ERROR(text) ;
                    return ;
                                  }

                               }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_MissileLego::~RSS_Module_MissileLego(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_MissileLego::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_MissileLego *object ;
                    char  models_list[4096] ;
                    char *end ;
                     int  i ;
                     int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define       PAR             object->Parameters
#define       PAR_CNT         object->Parameters_cnt
 
/*--------------------------------------------------- Проверка имени */

//    if(data->name[0]==0) {                                           /* Если имя НЕ задано */
//              SEND_ERROR("Секция MISSILE LEGO: Не задано имя объекта") ;
//                                return(NULL) ;
//                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция MISSILE LEGO: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- Считывание описания обьекта */
/*- - - - - - - - - - - - Если модель задана названием и библиотекой */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* Если модель НЕ задано */
              SEND_ERROR("Секция MISSILE LEGO: Не задана модель объекта") ;
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
              SEND_ERROR("Секция MISSILE LEGO: Неизвестная модель тела") ;
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

              SEND_ERROR("Секция MISSILE LEGO: Несоответствие числа параметров модели") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_MissileLego ;
    if(object==NULL) {
              SEND_ERROR("Секция MISSILE LEGO: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }

       strcpy(object->model_path, data->path) ;

/*------------------------------------- Сохранения списка параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция MISSILE LEGO: Переполнение памяти") ;
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
                         SEND_ERROR("Секция MISSILE LEGO: Переполнение памяти") ;
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
                                         data->sections[i].decl, "MissileLego.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- Введение объекта в общий список */

  if(data->name[0]!=0) {                                            /* Если имя задано... */

       strcpy(object->Name, data->name) ;
              object->Module=this ;

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция MISSILE LEGO: Переполнение памяти") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                          (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;
                       }
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

     int  RSS_Module_MissileLego::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  НУР, 3-х компонентная контейнерная модель", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_MissileLego::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MISSILE-LEGO"
#define  _SECTION_SHRT_NAME   "MISSILE-LEGO"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object MissileLego:") ;
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
       if(status)  SEND_ERROR("Секция MISSILE LEGO: Неизвестная команда") ;
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

    void  RSS_Module_MissileLego::vReadSave(std::string *data)

{
                   char *buff ;
                    int  buff_size ;
         RSS_Model_data  create_data ;
 RSS_Object_MissileLego *object ;
                   char  name[128] ;
                   char *entry ;
                   char *end ;
                    int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE MISSILE LEGO\n", 
                      strlen("#BEGIN MODULE MISSILE LEGO\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT MISSILE LEGO\n", 
                      strlen("#BEGIN OBJECT MISSILE LEGO\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT MISSILE LEGO\n", 
              strlen("#BEGIN OBJECT MISSILE LEGO\n"))) {            /* IF.1 */
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
                object=(RSS_Object_MissileLego *)vCreateObject(&create_data) ;
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

    void  RSS_Module_MissileLego::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE MISSILE LEGO\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_MissileLego::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_MissileLego_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> [<Модель> [<Список параметров>]]               */

  int  RSS_Module_MissileLego::cCreate(char *cmd)

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
                                     Object_MissileLego_Create_dialog, 
                                    (LPARAM)&data                     ) ;
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

  int  RSS_Module_MissileLego::cInfo(char *cmd)

{
                   char  *name ;
 RSS_Object_MissileLego  *object ;
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

       object=(RSS_Object_MissileLego *)FindObject(name, 1) ;       /* Ищем объект по имени */
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
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev, object->a_roll,
                   sqrt(object->x_velocity*object->x_velocity+
                        object->y_velocity*object->y_velocity+
                        object->z_velocity*object->z_velocity ),
                        object->x_velocity, object->y_velocity, object->z_velocity,
                        object->owner                    ) ;

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

  int  RSS_Module_MissileLego::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                   char  *pars[_PARS_MAX] ;
                   char  *name ;
                   char  *copy ;
 RSS_Object_MissileLego  *missile ;
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

       missile=(RSS_Object_MissileLego *)FindObject(name, 1) ;      /* Ищем объект-цель по имени */
    if(missile==NULL)  return(-1) ;

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

            object=missile->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции OWNER                   */
/*								    */
/*       OWNER <Имя> <Носитель>                                     */

  int  RSS_Module_MissileLego::cOwner(char *cmd)

{
#define   _PARS_MAX  10

                   char  *pars[_PARS_MAX] ;
                   char  *name ;
                   char  *owner ;
 RSS_Object_MissileLego  *missile ;
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

       missile=(RSS_Object_MissileLego *)FindObject(name, 1) ;      /* Ищем объект-цель по имени */
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

  int  RSS_Module_MissileLego::cTarget(char *cmd)

{
#define   _PARS_MAX  10

                   char  *pars[_PARS_MAX] ;
                   char  *name ;
                   char  *target ;
 RSS_Object_MissileLego  *missile ;
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

       missile=(RSS_Object_MissileLego *)FindObject(name, 1) ;          /* Ищем объект-цель по имени */
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
/*		      Реализация инструкции LEGO                    */
/*								    */
/*     LEGO <Имя объекта>                                           */
/*     LEGO <Имя объекта> <Имя компонента> <Тип компонента>         */

  int  RSS_Module_MissileLego::cLego(char *cmd)

{
#define   _PARS_MAX  10

                   char *pars[_PARS_MAX] ;
                   char *name ;
                   char *unit_name ;
                   char *unit_type ;
 RSS_Object_MissileLego *object ;
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
                                 "Например: LEGO <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_MissileLego *)FindObject(name, 1) ;       /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

   if(unit_name==NULL ||
      unit_type==NULL   ) {

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_LEGO", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_MissileLego_Lego_dialog, 
                                      (LPARAM)object               ) ;
         return((int)status) ;

                          }
/*-------------------------------- Создание и регистрация компонента */

   if(stricmp(unit_name, "warhead") &&
      stricmp(unit_name, "homing" ) &&
      stricmp(unit_name, "engine" ) &&
      stricmp(unit_name, "control") &&
      stricmp(unit_name, "model"  )   ) {
              SEND_ERROR("Допустимы следующие имена lego-компонентов: WARHEAD, HOMING, ENGINE, CONTROL и MODEL") ;
                     return(-1) ;
                                        }

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
/*		      Реализация инструкции TRACE                   */
/*								    */
/*       TRACE <Имя> [<Длительность>]                               */

  int  RSS_Module_MissileLego::cTrace(char *cmd)

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
 RSS_Object_MissileLego *object ;
                   char  text[1024] ;
                   char *end ;
                    int  quit_flag ;
                    int  status ;
                    int  i ;

/*---------------------------------------- Разборка командной строки */

                     trace_time=0. ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          quit_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quit_flag=1 ;

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
               if(!quit_flag)  SEND_ERROR("Время трассировки - 60 секунд") ;
                                 }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: TRACE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_MissileLego *)FindObject(name, 1) ;       /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------ Контроль носителя */

       object->o_owner=FindObject(object->owner, 0) ;               /* Ищем носитель по имени */
    if(object->o_owner==NULL)  return(-1) ;

/*------------------------------------------------------ Трассировка */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

         object->vCalculateStart(0.) ;

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
         status=object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
                object->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

                object->iShowTrace_("SHOW_TRACE") ;                 /* Отображение траектории */
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
      if(status)  break ;  

       } while(1) ;                                                 /* END CIRCLE.1 - Цикл трассировки */

   if(!quit_flag) {
              sprintf(text, "Трассировкa завершена за %lf секунд", time_c) ;
           SEND_ERROR(text) ;
                  }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SPAWN                   */
/*								    */
/*       SPAWN <Имя> <Ёмкость залпа> <Периодичность запуска>        */

  int  RSS_Module_MissileLego::cSpawn(char *cmd)

{
#define   _PARS_MAX  10

                   char *pars[_PARS_MAX] ;
                   char *name ;
                 double  spawn_max ;
                 double  spawn_period ;
                 double  time_0 ;        /* Стартовое время расчета */ 
                 double  time_1 ;        /* Текущее время */ 
                 double  time_c ;        /* Абсолютное время расчета */ 
                 double  time_s ;        /* Последнее время отрисовки */ 
                 double  time_w ;        /* Время ожидания */ 
                 double  time_z ;        /* Время очередного запуска */ 
 RSS_Object_MissileLego *object ;
             RSS_Object *clone ;
                    int  break_mark[1000] ;
                    int  break_cnt ;
                   char  text[1024] ;
                   char *end ;
                    int  quit_flag ;
                    int  status ;
                    int  n ;
                    int  i ;

/*---------------------------------------- Разборка командной строки */

                     spawn_max   =0. ;
                     spawn_period=0. ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                          quit_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quit_flag=1 ;

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

           if( pars[1]==NULL ||
              *pars[1]==  0    ) {
                                    SEND_ERROR("Не задана ёмкость залпа") ;
                                                           return(-1) ;
                                 }
           if( pars[2]==NULL ||
              *pars[2]==  0    ) {
                                    SEND_ERROR("Не задана периодичность пуска") ;
                                                           return(-1) ;
                                 }

                     name=pars[0] ;

                spawn_max=strtoul(pars[1], &end, 10) ;
           if(*end!=0) {
                           SEND_ERROR("Некорректное значение ёмкости залпа") ;
                                            return(-1) ;
                       }

                spawn_period=strtod(pars[2], &end) ;
           if(*end!=0) {
                           SEND_ERROR("Некорректное значение периодичности пуска") ;
                                            return(-1) ;
                       }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: SPAWN <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_MissileLego *)FindObject(name, 1) ;       /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------ Контроль носителя */

       object->o_owner=FindObject(object->owner, 0) ;               /* Ищем носитель по имени */
    if(object->o_owner==NULL)  return(-1) ;

/*---------------------------------------- Очистка предыдущего залпа */

                   object->iClearSpawn() ;

              memset(break_mark, 0, sizeof(break_mark)) ;
                     break_cnt=0 ;

/*------------------------------------------------------ Трассировка */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;
              time_z=0. ;

    do {                                                            /* CIRCLE.1 - Цикл трассировки */
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отработка времени */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

//         if(time_1-time_0>trace_time)  break ;                    /* Если время трассировки закончилось */

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - - - - Моделирование движения */
#define  CLONE(k)  ((RSS_Object_MissileLego *)object->mSpawn[k])

       for(n=0 ; n<object->mSpawn_cnt ; n++) if(!break_mark[n]) {

            status=CLONE(n)->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         if(status) {
                       break_mark[n]=1 ;
                       break_cnt++ ;
                    }

                  CLONE(n)->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

                  CLONE(n)->iShowTrace_("SHOW_TRACE") ;             /* Отображение траектории */

         for(i=0 ; i<CLONE(n)->Features_cnt ; i++) {                /* Отображение объекта */
           CLONE(n)->Features[i]->vBodyBasePoint(NULL, CLONE(n)->x_base, 
                                                       CLONE(n)->y_base, 
                                                       CLONE(n)->z_base ) ;
           CLONE(n)->Features[i]->vBodyAngles   (NULL, CLONE(n)->a_azim, 
                                                       CLONE(n)->a_elev, 
                                                       CLONE(n)->a_roll ) ;
                                                   }
                                                                } 

#undef  CLONE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Пуск */
       while(spawn_max>object->mSpawn_cnt &&
              time_z  <  time_c             ) {

                clone=object->vCopy(NULL) ;

                clone->vCalculateStart(time_z) ;
                clone->vCalculate     (time_z, time_c, NULL, 0) ;
                clone->vCalculateShow (time_z, time_c) ;

               object->mSpawn=(RSS_Object **)
                                realloc(object->mSpawn, sizeof(*object->mSpawn)*(object->mSpawn_cnt+1)) ;

               object->mSpawn[object->mSpawn_cnt]=clone ;
                              object->mSpawn_cnt++ ;

                       time_z+=spawn_period ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка сцены */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       if(object->mSpawn_cnt==spawn_max &&
          object->mSpawn_cnt==break_cnt   )  break ;  

       } while(1) ;                                                 /* END CIRCLE.1 - Цикл трассировки */

   if(!quit_flag) {
              sprintf(text, "Трассировкa завершена за %lf секунд", time_c) ;
           SEND_ERROR(text) ;
                  }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*             Поиск обьекта типа MISSILE LEGO по имени             */

  RSS_Object *RSS_Module_MissileLego::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Missile-Lego")) {

           SEND_ERROR("Объект не является объектом типа MISSILE LEGO") ;
                            return(NULL) ;
                                                  }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*                                                                   */
/*                 Добавление компонента к объекту                   */

  class RSS_Unit *RSS_Module_MissileLego::AddUnit(RSS_Object_MissileLego *object, char *unit_name, char *unit_type, char *error)

{
   RSS_Kernel *unit_module ;
     RSS_Unit *unit ;
          int  i ;

/*---------------------------------------- Контроль имени компонента */

  if(stricmp(unit_name, "warhead") &&
     stricmp(unit_name, "homing" ) &&
     stricmp(unit_name, "engine" ) &&
     stricmp(unit_name, "control") &&
     stricmp(unit_name, "model"  )   ) {                            /* Кроме имен lego-компонентов */

   for(i=0 ; i<object->Units.List_cnt ; i++)
     if(!stricmp(unit_name, object->Units.List[i].object->Name))  break ;

      if(i<object->Units.List_cnt) {
            strcpy(error, "В состав объекта уже включен компонент с таким именем") ;
                                     return(NULL) ;
                                   }
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
/*- - - - - - - - - - - - - - - - - - - Регистрация Lego-компонентов */
  if(!stricmp(unit_name, "warhead")) {
                                        object->unit_warhead=(RSS_Unit_WarHead *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "homing" )) {
                                        object->unit_homing=(RSS_Unit_Homing *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "engine" )) {
                                        object->unit_engine=(RSS_Unit_Engine *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "control")) {
                                        object->unit_control=(RSS_Unit_Control *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "model"  )) {
                                        object->unit_model=(RSS_Unit_Model *)unit ;
                                     }
/*- - - - - - - - - - -  Регистрация Лего- и "свободных" компонентов */
                  object->Units.Add(unit, "") ;

/*-------------------------------------------------------------------*/

  return(unit) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "РАКЕТА-ЛЕГО"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_MissileLego::RSS_Object_MissileLego(void)

{
   strcpy(Type, "Missile-Lego") ;

          Module=&ProgramModule ;

    Context        =new RSS_Transit_MissileLego ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     unit_warhead=NULL ;
     unit_homing =NULL ;
     unit_engine =NULL ;
     unit_control=NULL ;
     unit_model  =NULL ;

       mSpawn    =NULL ;
       mSpawn_cnt= 0  ;

  battle_state   = 0 ; 

       x_base    = 0. ;
       y_base    = 0. ;
       z_base    = 0. ;
       a_azim    = 0. ;
       a_elev    = 0. ;
       a_roll    = 0. ;
       x_velocity= 0. ;
       y_velocity= 0. ;
       z_velocity= 0. ;

  memset(owner,  0, sizeof(owner )) ;
       o_owner =NULL ;   

      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=RGB(0, 0, 127) ;
      mTrace_dlist=  0 ;  

      hDropsViewWnd=NULL ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_MissileLego::~RSS_Object_MissileLego(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Object_MissileLego::vFree(void)

{
  int  i ;


           iClearSpawn() ;

       iSaveTracePoint("CLEAR") ;
           iShowTrace_("CLEAR_TRACE") ;

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
/*	      Освобождение ресурсов моделирования залпа             */

  void   RSS_Object_MissileLego::iClearSpawn(void)

{
  int  i ;


  if(this->mSpawn!=NULL) {
    for(i=0 ; i<this->mSpawn_cnt ; i++) {
                                           delete (RSS_Object_MissileLego *)this->mSpawn[i] ;
                                        }

                             free(this->mSpawn) ;
                                  this->mSpawn    =NULL ;
                                  this->mSpawn_cnt=  0 ;
                         }
}


/********************************************************************/
/*								    */
/*                        Копировать объект		            */

    class RSS_Object *RSS_Object_MissileLego::vCopy(char *name)

{
         RSS_Model_data  create_data ;
 RSS_Object_MissileLego *object ;
        RSS_Feature_Hit *hit_1 ;
        RSS_Feature_Hit *hit_2 ;
               RSS_Unit *unit ;
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

       object=(RSS_Object_MissileLego *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;

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

/*----------------------------------------- Копирование лего-модулей */

      if(this->unit_warhead!=NULL) {
                         unit=(RSS_Unit *)this->unit_warhead->vCopy(NULL) ;
                  strcpy(unit->Name, "warhead") ;
                         unit->Owner=object ;

                 object->unit_warhead=(RSS_Unit_WarHead *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_homing !=NULL) {
                         unit=(RSS_Unit *)this->unit_homing->vCopy(NULL) ;
                  strcpy(unit->Name, "homing") ;
                         unit->Owner=object ;

                 object->unit_homing=(RSS_Unit_Homing *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_engine !=NULL) {
                         unit=(RSS_Unit *)this->unit_engine->vCopy(NULL) ;
                  strcpy(unit->Name, "engine") ;
                         unit->Owner=object ;

                 object->unit_engine=(RSS_Unit_Engine *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_control!=NULL) {
                         unit=(RSS_Unit *)this->unit_control->vCopy(NULL) ;
                  strcpy(unit->Name, "control") ;
                         unit->Owner=object ;

                 object->unit_control=(RSS_Unit_Control *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_model  !=NULL) {
                         unit=(RSS_Unit *)this->unit_model->vCopy(NULL) ;
                  strcpy(unit->Name, "model") ;
                         unit->Owner=object ;

                 object->unit_model=(RSS_Unit_Model *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_MissileLego::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT MISSILE LEGO\n" ;

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
/*                        Специальные действия                      */

     int  RSS_Object_MissileLego::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object_MissileLego::vCalculateStart(double t)
{
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
/*-------------------------------------------- Обработка компонентов */

      if(this->unit_engine !=NULL)  this->unit_engine ->vCalculateStart(t) ;
      if(this->unit_warhead!=NULL)  this->unit_warhead->vCalculateStart(t) ;
      if(this->unit_homing !=NULL)  this->unit_homing ->vCalculateStart(t) ;
      if(this->unit_control!=NULL)  this->unit_control->vCalculateStart(t) ;
      if(this->unit_model  !=NULL)  this->unit_model  ->vCalculateStart(t) ;

    for(i=0 ; i<this->Units.List_cnt ; i++)                         /* Кроме LEGO-компонентов */
      if(stricmp(this->Units.List[i].object->Name, "engine" ) &&
         stricmp(this->Units.List[i].object->Name, "warhead") &&
         stricmp(this->Units.List[i].object->Name, "homing" ) &&
         stricmp(this->Units.List[i].object->Name, "control") &&
         stricmp(this->Units.List[i].object->Name, "model"  )   )
                   this->Units.List[i].object->vCalculateStart(t) ;

/*------------------------------------------------ Очистка контекста */

    this->iSaveTracePoint("CLEAR") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Object_MissileLego::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Unit_Engine_Thrust  thrust[10] ;
                     int  thrust_cnt ;
                     int  mass_use ;
                  double  mass ;
               RSS_Point  mass_point ;
                     int  mi_use ;
                  double  mi_x ;
                  double  mi_y ;
                  double  mi_z ;
                    char  homing_control[1024] ;
                     int  status ;

/*------------------------------------------------------- Подготовка */

            thrust_cnt=0 ;
              mass_use=0 ;
                mi_use=0 ;

/*---------------------------------------------------- Моделирование */

/*- - - - - - - - - - - - - - - - - - - - - - - -  Система наведения */
   if(this->unit_homing !=NULL) {
                                    homing_control[0]=0 ;          

     if(this->unit_control!=NULL)  this->unit_control->vGetHomingControl(homing_control) ;

        this->unit_homing->vSetHomingControl(homing_control) ;
        this->unit_homing->vCalculate       (t1, t2, callback, cb_size) ;

                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Двигатель */
   if(this->unit_engine !=NULL) {

                       this->unit_engine->vCalculate      (t1, t2, callback, cb_size) ;

            thrust_cnt=this->unit_engine->vGetEngineThrust(thrust) ;
              mass_use=this->unit_engine->vGetEngineMass  (&mass, &mass_point) ;
                mi_use=this->unit_engine->vGetEngineMI    (&mi_x, &mi_y, &mi_z) ;

                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - Система управления */
/*- - - - - - - - - - - - - - - - - - -  Динамическая модель объекта */
   if(this->unit_model  !=NULL) {

          if(thrust_cnt)  this->unit_model->vSetEngineThrust(thrust, thrust_cnt) ;
          if(  mass_use)  this->unit_model->vSetEngineMass  (mass, &mass_point) ;
          if(    mi_use)  this->unit_model->vSetEngineMI    (mi_x, mi_y, mi_z) ;

                          this->unit_model->vCalculate(t1, t2, callback, cb_size) ;

                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Боевая часть */
   if(this->unit_warhead!=NULL) {

            status=this->unit_warhead->vCalculate(t1, t2, callback, cb_size) ;
         if(status)  return(1) ;

                                } 
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object_MissileLego::vCalculateShow(double  t1, double  t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* Сохранение точки траектории */  

         this->iShowTrace_("SHOW_TRACE") ;                          /* Отображение траектории */

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

  int  RSS_Object_MissileLego::iSaveTracePoint(char *action)

{
/*------------------------------------------------- Сброс траектории */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- Довыделение буфера */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_MissileLegoTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_MissileLegoTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("MISSILE-LEGO.iSaveTracePoint@"
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
                  mTrace[mTrace_cnt].color     =RGB(  0, 0, 127) ;

                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           Отображение траектории с передачей контекста           */

  void  RSS_Object_MissileLego::iShowTrace_(char *action)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*								     */
/*                     Отображение траектории                        */

  void  RSS_Object_MissileLego::iShowTrace(char *action)

{
       int  status ;
       int  i ;

/*-------------------------------------- Удаление дисплейного списка */

  if(!stricmp(action, "CLEAR_TRACE")) {

     if(this->mTrace_dlist)                                         /* Освобождаем дисплейный список */                                       
             RSS_Kernel::display.ReleaseList(this->mTrace_dlist) ;

                                               return ;
                                      }
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

     RSS_Transit_MissileLego::RSS_Transit_MissileLego(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_MissileLego::~RSS_Transit_MissileLego(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_MissileLego::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE" ))  ((RSS_Object_MissileLego *)object)->iShowTrace(action) ;
   if(!stricmp(action, "CLEAR_TRACE"))  ((RSS_Object_MissileLego *)object)->iShowTrace(action) ;

   return(0) ;
}

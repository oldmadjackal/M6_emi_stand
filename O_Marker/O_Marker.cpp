/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "МАРКЕР"  		    */
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
#include <sys\stat.h>

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_Marker.h"

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

     static   RSS_Module_Marker  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_MARKER_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_MARKER_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "МАРКЕР"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Marker_instr  RSS_Module_Marker_InstrList[]={

 { "help",   "?",  "#HELP   - список доступных команд", 
                    NULL,
                   &RSS_Module_Marker::cHelp   },
 { "create", "cr", "#CREATE - создать объект",
                   " CREATE <Имя> [<Модель> [<Цвет>]]\n"
                   "   Создает именованный маркер заданных формы и цвета",
                   &RSS_Module_Marker::cCreate },
 { "info",   "i",  "#INFO - выдать информацию по объекту",
                   " INFO <Имя> \n"
                   "   Выдать основную нформацию по объекту в главное окно\n"
                   " INFO/ <Имя> \n"
                   "   Выдать полную информацию по объекту в отдельное окно",
                   &RSS_Module_Marker::cInfo },
 { "base",   "b", "#BASE - задать базовую точку объекта",
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
                   &RSS_Module_Marker::cBase },
 { "angle",  "a", "#ANGLE - задать углы ориентации объекта",
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
                   &RSS_Module_Marker::cAngle },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Marker_instr *RSS_Module_Marker::mInstrList=NULL ;

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Marker::RSS_Module_Marker(void)

{
	   keyword="EmiStand" ;
    identification="Marker" ;
          category="Object" ;

        mInstrList=RSS_Module_Marker_InstrList ;

          mSize=50. ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Marker::~RSS_Module_Marker(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Marker::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_Marker *object ;
               char *end ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define  COLOR  data->pars[0].value
 
/*--------------------------------------------------- Проверка имени */

    if(data->name[0]==0) {                                           /* Если имя НЕ задано */
              SEND_ERROR("Секция MARKER: Не задано имя объекта") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция MARKER: Объект с таким именем уже существует") ;
                                return(NULL) ;
                                                    }
/*--------------------------------------- Определение модели маркера */

    if(data->model[0]==0) {                                         /* Если модель по-умолчанию... */
                             strcpy(data->model, "Cross") ;
                          }

    if(stricmp(data->model, "Cross"  ) &&
       stricmp(data->model, "Pyramid")   ) {
                 
              SEND_ERROR("Секция MARKER: Неизвестная модель маркера") ;
                                              return(NULL) ;
                                           }
/*---------------------------------------- Определение цвета маркера */

   if(COLOR[0]==0) {                                                /* Если цвет по-умолчанию... */
                      strcpy(COLOR, "Red") ;
                   }

   if(stricmp(COLOR, "Red"  ) &&
      stricmp(COLOR, "Green") && 
      stricmp(COLOR, "Blue")    ) {
                 
              SEND_ERROR("Секция MARKER: Неизвестный цвет маркера") ;
                                            return(NULL) ;
                                  }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_Marker ;
    if(object==NULL) {
              SEND_ERROR("Секция MARKER: Недостаточно памяти для создания объекта") ;
                        return(NULL) ;
                     }

     strncpy(object->model, data->model, sizeof(object->model)-1) ;
             object->size=mSize ;

   if(!stricmp(COLOR, "Red"  ))  object->color=RGB(255,   0,   0) ;
   if(!stricmp(COLOR, "Green"))  object->color=RGB(  0, 255,   0) ;
   if(!stricmp(COLOR, "Blue" ))  object->color=RGB(  0,   0, 255) ;

/*------------------------------------- Сохранения списка параметров */

#define  PAR      object->Parameters
#define  PAR_CNT  object->Parameters_cnt
/*- - - - - - - - - - - - - - - - - - - - - - - -  Заносим параметры */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция MARKER: Переполнение памяти") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
            sprintf( PAR[PAR_CNT].name, "PAR%d", i+1) ;
                     PAR[PAR_CNT].value=strtod(data->pars[i].value, &end) ;
                         PAR_CNT++ ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - Терминируем пустой записью */
           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция MARKER: Переполнение памяти") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef  PAR
#undef  PAR_CNT

/*--------------------------------------------- Формирование маркера */

                      object->iFormMarker_() ;

/*------------------ Формирование позиционирующего заголовка маркера */

                      object->iPlaceMarker_() ;

/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция MARKER: Переполнение памяти") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

             strcpy(object->Name, data->name) ;
                    object->Module=this ;


        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT
#undef     COLOR

  return(object) ;
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Marker::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Маркер", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_Marker::vShow(char *layer)

{
     int  status ;
     int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
  
/*----------------------------------------- Масштабирование картинки */

  if(!stricmp(layer, "ZOOM")) {

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - Перебор контекстов отображения */

     for(i=0 ; i<OBJECTS_CNT ; i++) {                               /* LOOP - Перебор маркеров */

       if(stricmp(OBJECTS[i]->Type, "Marker"))  continue ;

           ((RSS_Object_Marker *)OBJECTS[i])->iPlaceMarker() ;
                                    }                               /* ENDLOOP - Перебор маркеров */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - Перебор контекстов отображения */

                              }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Marker::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MARKER"
#define  _SECTION_SHRT_NAME   "M"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Marker:") ;
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
       if(status)  SEND_ERROR("Секция MARKER: Неизвестная команда") ;
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

    void  RSS_Module_Marker::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
  RSS_Object_Marker *object ;
               char *entry ;
               char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE MARKER\n", 
                      strlen("#BEGIN MODULE MARKER\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT MARKER\n", 
                      strlen("#BEGIN OBJECT MARKER\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT MARKER\n", 
              strlen("#BEGIN OBJECT MARKER\n"))) {                  /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  Извлечение параметров */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* Извлекаем имя объекта */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;

                                     entry=strstr(buff, "MODEL=") ; /* Извлекаем модель объекта */
           strncpy(create_data.model, entry+strlen("MODEL="),
                                       sizeof(create_data.model)-1) ;
        end=strchr(create_data.model, '\n') ;
       *end= 0 ;
/*- - - - - - - - - - - - - - - Проверка повторного создания объекта */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
                object=(RSS_Object_Marker *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - - - - - - - - - - - Параметры визуализации */
       entry=strstr(buff, "SIZE=") ;  object->size=atof(entry+strlen("SIZE=")) ;
       entry=strstr(buff, "COLOR=") ; object->color=strtoul(entry+strlen("COLOR="), &end, 16) ;
/*- - - - - - - - - - - - Пропись базовой точки и ориентации объекта */
       entry=strstr(buff, "X_BASE=") ; object->x_base=atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->y_base=atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->z_base=atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->a_azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->a_elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->a_roll=atof(entry+strlen("A_ROLL=")) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     object->iPlaceMarker_() ;
                                               }                    /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Marker::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE MARKER\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Marker::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Marker_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> [<Модель> [<Цвет>]]                            */

  int  RSS_Module_Marker::cCreate(char *cmd)

{
 RSS_Model_data  data ;
     RSS_Object *object ;
           char *name ;
           char *model ;
           char *pars[4] ;
           char *end ;
            int  i ;

/*-------------------------------------- Дешифровка командной строки */

                            model ="" ;
     for(i=0 ; i<4 ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя объекта */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                 model=end+1 ;                                      /* Извлекаем название модели */
                   end=strchr(model, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<4 ; i++) {                                         /* Извлекаем параметры */
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

/*------------------------------------------------- Создание маркера */

      object=vCreateObject(&data) ;
   if(object==NULL)  return(-1) ;

/*------------------------------------------------ Отображение сцены */

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Marker::cInfo(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *name ;
  RSS_Object_Marker  *object ;
                int   all_flag ;   /* Флаг режима полной информации */
               char  *end ;
        std::string   info ;
        std::string   f_info ;
               char   text[4096] ;

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

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 7.3lf\r\n" 
                    "     Y % 7.3lf\r\n" 
                    "     Z % 7.3lf\r\n"
                    "Ang. A % 7.3lf\r\n" 
                    "     E % 7.3lf\r\n" 
                    "     R % 7.3lf\r\n"
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev, object->a_roll
                    ) ;

           info=text ;

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
/*		      Реализация инструкции BASE                    */
/*								    */
/*        BASE    <Имя> <X> <Y> <Z>                                 */
/*        BASE/X  <Имя> <X>                                         */
/*        BASE/+X <Имя> <X>                                         */
/*        BASE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        BASE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Marker::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
  RSS_Object_Marker  *object ;
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

       object=FindObject(name) ;                                    /* Ищем объект по имени */
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
/*------------------------------------------------ Отображение сцены */

            object->iPlaceMarker_() ;

              this->kernel->vShow("REFRESH") ;

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

  int  RSS_Module_Marker::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
  RSS_Object_Marker  *object ;
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

       object=FindObject(name) ;                                    /* Ищем объект по имени */
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

/*------------------------------------------------ Отображение сцены */

            object->iFormMarker_() ;
            object->iPlaceMarker_() ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа MARKER по имени               */

  RSS_Object_Marker *RSS_Module_Marker::FindObject(char *name)

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

     if(stricmp(OBJECTS[i]->Type, "Marker")) {

           SEND_ERROR("Объект не является объектом типа MARKER") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_Marker *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "МАРКЕР"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_Marker::RSS_Object_Marker(void)

{
   strcpy(Type, "Marker") ;

    Context        =new RSS_Transit_Marker ;
    Context->object=this ;

       Parameters    =NULL ;
       Parameters_cnt=  0 ;

           x_base    =0 ;
           y_base    =0 ;
           z_base    =0 ;

           a_azim    =0 ;
           a_elev    =0 ;
           a_roll    =0 ;

           x_velocity=0 ;
           y_velocity=0 ;
           z_velocity=0 ;

           dlist1_idx=0 ;
           dlist2_idx=0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Marker::~RSS_Object_Marker(void)

{
      if(this->dlist1_idx)
           RSS_Kernel::display.ReleaseList(this->dlist1_idx) ;

      if(this->dlist2_idx)
           RSS_Kernel::display.ReleaseList(this->dlist2_idx) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_Marker::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT MARKER\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n",       this->Name  ) ;  *text+=field ;
    sprintf(field, "MODEL=%s\n",      this->model ) ;  *text+=field ;
    sprintf(field, "COLOR=%x\n",      this->color ) ;  *text+=field ;
    sprintf(field, "SIZE=%.10lf\n",   this->size  ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->x_base) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->y_base) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->z_base) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->a_azim) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->a_elev) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->a_roll) ;  *text+=field ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                Получить список параметров управления             */

   int  RSS_Object_Marker::vListControlPars(RSS_ControlPar *list)

{
 RSS_ControlPar  pars[6]={ 
     { "x_base", "Базовая точка", "X",               "double" },
     { "y_base", "Базовая точка", "Y",               "double" },
     { "z_base", "Базовая точка", "Z",               "double" },
     { "a_azim", "Ориентация",    "Азимут",          "double" },
     { "a_elev", "Ориентация",    "Угол возвышения", "double" },
     { "a_roll", "Ориентация",    "Угол крена",      "double" },
                         } ;


     if(list!=NULL)  memcpy(list, pars, sizeof(pars)) ;

   return(6) ;
}


/********************************************************************/
/*								    */
/*               Установить значение параметра управления           */

   int  RSS_Object_Marker::vSetControlPar(RSS_ControlPar *par)   

{
   double  value ;
     char  text[1024] ;


   if(!stricmp(par->type, "double"))  value=*((double *)par->value) ;
   else                              {
               sprintf(text, "Unknown value type %s for object %s", par->type, this->Name) ;
            SEND_ERROR(text) ;
                return(-1) ;
                                     }

        if(!stricmp(par->link, "x_base"))  this->x_base=value ;
   else if(!stricmp(par->link, "y_base"))  this->y_base=value ;
   else if(!stricmp(par->link, "z_base"))  this->z_base=value ;
   else if(!stricmp(par->link, "a_azim"))  this->a_azim=value ;
   else if(!stricmp(par->link, "a_elev"))  this->a_elev=value ;
   else if(!stricmp(par->link, "a_roll"))  this->a_roll=value ;
   else                                   {
               sprintf(text, "Unknown control parameter %s for object %s", par->link, this->Name) ;
            SEND_ERROR(text) ;
                return(-1) ;
                                          }

     return(0) ;
}


/********************************************************************/
/*								    */
/*           Задание формы маркера с передачей контекста            */

  int  RSS_Object_Marker::iFormMarker_(void)

{
       strcpy(Context->action, "FORM") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Задание формы маркера                         */

  int  RSS_Object_Marker::iFormMarker(void)

{
  int  status ;


/*-------------------------------- Резервирование дисплейного списка */

     if(dlist2_idx==0)  dlist2_idx=RSS_Kernel::display.GetList(1) ;

     if(dlist2_idx==0)  return(-1) ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*--------------------------------------- Формирование метки маркера */

             glNewList(dlist2_idx, GL_COMPILE) ;                    /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
//            glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
//         glDepthMask(0) ;                                         /* Откл.запись Z-буфера */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Крест */
    if(!stricmp(model, "Cross"  )) {

 double  s ;
                     s=0.5 ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 1.) ;

                       glBegin(GL_LINES) ;
                    glVertex3d(-s,  0,  0) ;
                    glVertex3d( s,  0,  0) ;
                    glVertex3d( 0, -s,  0) ;
                    glVertex3d( 0,  s,  0) ;
                    glVertex3d( 0,  0, -s) ;
                    glVertex3d( 0,  0,  s) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - Пирамида */
    if(!stricmp(model, "Pyramid")) {

 double  s ;
 double  h ;

                     s=0.5 ;
                     h=1.0 ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 0.2) ;

                       glBegin(GL_POLYGON) ;
                    glVertex3d( s, -h, -s) ;
                    glVertex3d(-s, -h, -s) ;
                    glVertex3d( 0, -h,  s) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d( s, -h, -s) ;
                    glVertex3d(-s, -h, -s) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d(-s, -h, -s) ;
                    glVertex3d( 0, -h,  s) ;
              	         glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d( 0, -h,  s) ;
                    glVertex3d( s, -h, -s) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//         glDepthMask(1) ;                                         /* Вкл.запись Z-буфера */
//           glDisable(GL_BLEND) ;                                  /* Откл.смешивание цветов */
             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           Позиционирование маркера с передачей контекста         */

  int  RSS_Object_Marker::iPlaceMarker_(void)

{
       strcpy(Context->action, "PLACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Позиционирование маркера                      */

  int  RSS_Object_Marker::iPlaceMarker(void)

{
  double  zoom ;  
     int  status ;


/*-------------------------------- Резервирование дисплейного списка */

     if(dlist1_idx==0)  dlist1_idx=RSS_Kernel::display.GetList(0) ;

     if(dlist1_idx==0)  return(-1) ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*------------------ Формирование позиционирующей последовательности */

             glNewList(dlist1_idx, GL_COMPILE) ;                    /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(x_base, y_base, z_base) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/size ;
              glScaled(zoom, zoom, zoom) ;

            glCallList(dlist2_idx) ;                                /* Отрисовка маркера */

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

     RSS_Transit_Marker::RSS_Transit_Marker(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Marker::~RSS_Transit_Marker(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Marker::vExecute(void)

{
   if(!stricmp(action, "FORM" ))  ((RSS_Object_Marker *)object)->iFormMarker() ;
   if(!stricmp(action, "PLACE"))  ((RSS_Object_Marker *)object)->iPlaceMarker() ;

   return(0) ;
}



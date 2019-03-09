/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "СЦЕНАРНЫЙ БОЙ"    		    */
/*								    */
/********************************************************************/

#pragma warning( disable : 4996 )
#define  stricmp  _stricmp

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

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\F_Hit\F_Hit.h"

#include "T_Battle.h"


#define  _SECTION_FULL_NAME   "BATTLE"
#define  _SECTION_SHRT_NAME   "BAT"

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  CREATE_DIALOG  CreateDialogIndirectParam


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

     static   RSS_Module_Battle  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_BATTLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_BATTLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	       ОПИСАНИЕ КЛАССА ЗАДАЧИ "ВЫХОД В ТОЧКУ"              **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Battle_instr  RSS_Module_Battle_InstrList[]={

 { "help",       "?",  "#HELP (?) - список доступных команд", 
                        NULL,
                       &RSS_Module_Battle::cHelp   },
 { "clear",      "c",  "#CLEAR (C) - очистить контекст сценария боя и восстановить исходное положение", 
                        NULL,
                       &RSS_Module_Battle::cClear  },
 { "add",        "a",  "#ADD (A) - добавление объекта в сценарий боя", 
                       " ADD <Имя объекта>\n",
                       &RSS_Module_Battle::cAdd    },
 { "list",       "l",  "#LIST (L) - просмотр описания сценария и состава участников боя", 
                        NULL,
                       &RSS_Module_Battle::cList   },
 { "program",    "p",  "#PROGRAM (P) - загрузка сценария из файла", 
                       " PROGRAM <Имя файла>\n",
                       &RSS_Module_Battle::cProgram },
 { "run",        "r",  "#RUN (R) - запуск исполнения сценария боя", 
                       " RUN [<Число повторов>]\n"
                       "   Исполнение сценария в реальном времени\n"
                       " RUN/F [<Число повторов>]\n"
                       "   Исполнение сценария с максимальным ускорением времени\n",
                       &RSS_Module_Battle::cRun },
 { "map",        "m",  "#MAP (M) - отображение карты боя", 
                       " MAP [<Квадрант размещения>]\n"
                       "   Отобразить окно карты поля боя\n"
                       " MAP/C[Q] <Объект>[&] <Название цвета>\n"
                       "   Установить цвет объекта (объектов с заданным шаблоном имени) на карте: RED, GREEN, BLUE\n"
                       "   Kлюч Q блокирует выдачу предупреждающих сообщений\n"
                       " MAP/T[Q] <Объект>\n"
                       "   Трассировать объект\n"
                       "   Kлюч Q блокирует выдачу предупреждающих сообщений\n",
                       &RSS_Module_Battle::cMap },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Battle_instr *RSS_Module_Battle::mInstrList       =NULL ;

                               OBJ  RSS_Module_Battle::mObjects[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mObjects_cnt     =  0 ;

                             FRAME *RSS_Module_Battle::mSpawns[_SPAWNS_MAX] ;
                               int  RSS_Module_Battle::mSpawns_cnt      =  0 ;

                             FRAME *RSS_Module_Battle::mScenario        =NULL ;
                               int  RSS_Module_Battle::mScenario_cnt    =  0 ;

                               VAR *RSS_Module_Battle::mVariables       =NULL ;
                               int  RSS_Module_Battle::mVariables_cnt   =  0 ;
                              HWND  RSS_Module_Battle::mVariables_Window=NULL ;

                               int  RSS_Module_Battle::mHit_cnt ;

                              HWND  RSS_Module_Battle::mMapWindow ;
                               int  RSS_Module_Battle::mMapRegime ;
                            double  RSS_Module_Battle::mMapXmin ;
                            double  RSS_Module_Battle::mMapXmax ;
                            double  RSS_Module_Battle::mMapZmin ;
                            double  RSS_Module_Battle::mMapZmax ;
                             COLOR  RSS_Module_Battle::mMapColors[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mMapColors_cnt ;
                             TRACE  RSS_Module_Battle::mMapTraces[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mMapTraces_cnt ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Battle::RSS_Module_Battle(void)

{
  static  WNDCLASS  Map_wnd ;

/*---------------------------------------------------- Инициализация */

                keyword="EmiStand" ;
         identification="Battle" ;
               category="Task" ;

             mInstrList=RSS_Module_Battle_InstrList ;

                Context=new RSS_Transit_Battle ;

/*----------------------------------- Регистрация класса элемент Map */

          mMapRegime=_MAP_KEEP_RANGE ;

	Map_wnd.lpszClassName="Task_Battle_Map_class" ;
	Map_wnd.hInstance    = GetModuleHandle(NULL) ;
	Map_wnd.lpfnWndProc  = Task_Battle_Map_prc ;
	Map_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	Map_wnd.hIcon        =  NULL ;
	Map_wnd.lpszMenuName =  NULL ;
	Map_wnd.hbrBackground=  NULL ;
	Map_wnd.style        =    0 ;
	Map_wnd.hIcon        =  NULL ;

            RegisterClass(&Map_wnd) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Battle::~RSS_Module_Battle(void)

{
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Battle::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Сценарный бой", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Battle::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Battle::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

/*--------------------------------------------- Идентификация секции */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;
/*- - - - - - - - - - - - - - - - - - - -  Обработка адресных команд */
        if(command[0]=='&') {
                                end=command+1 ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  Обработка прямых команд */
   else if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                            }
/*- - - - - - - - - - - - - - - - - - - -  Обработка опросных команд */
   else                     {
                                end=command ;
                            }
/*----------------------- Включение/выключение режима прямой команды */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Включение */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Battle:") ;
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
                     *end!='.' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* Ищем команду в списке */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* Если такой команды нет... */

          status=this->kernel->vExecuteCmd(cmd, iface) ;            /*  Пытаемся передать модулю ядра... */
       if(status)  SEND_ERROR("Секция BATTLE: Неизвестная команда") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end, iface) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Battle::vReadSave(std::string *data)

{

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE BATTLE\n", 
                      strlen("#BEGIN MODULE BATTLE\n")) )  return ;

/*------------------------------------------------ Извлечение данных */

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Battle::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE BATTLE\n" ;

/*-------------------------------------------------- Данные описания */

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*             Выполнить действие в контексте потока                */

    void  RSS_Module_Battle::vChangeContext(void)

{
/*------------------------------------------ Вывод списка переменных */

   if(!stricmp(mContextAction, "VARS")) {

    this->mVariables_Window=
                     CREATE_DIALOG(GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_VARIABLES", RT_DIALOG),
           	                      NULL, Task_Battle_Vars_dialog, 
                                       (LPARAM)this ) ;
                        ShowWindow(this->mVariables_Window, SW_SHOW) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Battle::cHelp(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Battle_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ADD                     */
/*								    */
/*       ADD <Имя объекта>                                          */

  int  RSS_Module_Battle::cAdd(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  text[1024] ;
               char  *end ;
                int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

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

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ADD <Имя_объекта>") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(-1) ;
                       }
/*-------------------------------------------------- Пропись объекта */

    if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("Список объектов сценария слишком велик") ;
                                     return(-1) ;
                                   }

         if(OBJECTS[i]->battle_state!=_SPAWN_STATE) {               /* Если объект не создан в процессе боя... */
                   OBJECTS[i]->battle_state=_ACTIVE_STATE ;
                   OBJECTS[i]->vPush() ;
                                                    }

               mObjects[mObjects_cnt].object=OBJECTS[i] ;
                        mObjects_cnt++ ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CLEAR                   */
/*								    */
/*       CLEAR                                                      */

  int  RSS_Module_Battle::cClear(char *cmd, RSS_IFace *iface)

{
  int  i ;
  int  j ;

/*--------------------------------- Инициализация контекста объектов */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt

       for(i=0 ; i<OBJECTS_CNT ; i++) {

         if(OBJECTS[i]->battle_state==_ACTIVE_STATE) {

                                OBJECTS[i]->vPop() ;
                                OBJECTS[i]->vCalculateShow(0, 0) ;
                                                     }
         if(OBJECTS[i]->battle_state== _SPAWN_STATE) {

                                         OBJECTS[i]->vFree() ;      /* Освобождение ресурсов */
                                 delete  OBJECTS[i] ;

             for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;
                           OBJECTS_CNT-- ;
                                     i-- ;
                                                     }
                                      }

#undef    OBJECTS
#undef    OBJECTS_CNT

/*------------------------------------------------ Очистка контекста */

           mObjects_cnt=0 ;
            mSpawns_cnt=0 ;

/*------------------------------------- Инициализация объектов карты */

     for(i=0 ; i<mMapTraces_cnt ; i++) {

       if(mMapTraces[i].points_max!=0)  free(mMapTraces[i].points) ;
                                             mMapTraces[i].points    =NULL ;
                                             mMapTraces[i].points_max= 0 ;
                                             mMapTraces[i].points_cnt= 0 ;
                                       }

                 mMapTraces_cnt=0 ;

       if(this->mMapWindow!=NULL)  SendMessage(this->mMapWindow, WM_PAINT, NULL, NULL) ;

/*------------------------------------------------------ Перерисовка */

                       this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LIST                    */

  int  RSS_Module_Battle::cList(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			    (LPCDLGTEMPLATE)Resource("IDD_BATTLE", RT_DIALOG),
			       GetActiveWindow(), Task_Battle_View_dialog, (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PROGRAM                 */
/*								    */
/*  Структура файла сценария:                                       */
/*								    */
/*    {T|DT}=<метка времени> EXIT                                   */
/*    {T|DT}=<метка времени> START <Имя объекта> [<Время слежения>] */
/*    {T|DT}=<метка времени> STOP  <Имя объекта>                    */
/*    {T|DT}=<метка времени> KILL  <Имя объекта>                    */
/*    {T|DT}=<метка времени> EVENT <Имя объекта> <Событие>          */
/*    {T|DT}=<метка времени> SPAWN <Имя объекта> <Число> <Темп>     */
/*    {T|DT}=<метка времени> EXEC  <Системная команда>              */

  int  RSS_Module_Battle::cProgram(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

  RSS_IFace  iface_ ;
       char *pars[_PARS_MAX] ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char *end ;
        int  cnt ;
       char  text[1024] ;
       char *work ;
       char  desc[1024] ;
       char *words[30] ;
     double  value ;
        int  row ;
       char  error[1024] ;
        int  n ;
        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

/*--------------------------------------------- Контроль имени файла */

    if(pars[0]==NULL) {                                             /* Если имя файла не задано... */
                       SEND_ERROR("Не задано имя файла сценария.\n"
                                  "Например: PROGRAM <Файл сценария>") ;
                                     return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* Если имя не содержит расширения - */
         if(       end       ==NULL ||                              /*   - добавляем типовое             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".battle") ;

/*--------------------------------------------------- Открытие файла */

        file=fopen(path, "rt") ;                                    /* Открываем файла */
     if(file==NULL) {
                           sprintf(text, "Ошибка открытия файла: %s", path) ;
                        SEND_ERROR(text) ;
                          return(-1) ;
                    }
/*------------------------------------------ Определение числа строк */

          for(cnt=0 ; ; cnt++) {
                  end=fgets(text, sizeof(text)-1, file) ;           /* Считываем очередную строку */
               if(end==NULL)  break ;                               /* Если все считано... */
                               }
/*---------------------------------------- Подготовка массива данных */

        this->mScenario=(FRAME *)
                          this->gMemRealloc(this->mScenario, cnt*sizeof(FRAME), 
                                             "RSS_Module_Battle::mScenario", 0, 0) ;
     if(this->mScenario==NULL) {
                   sprintf(text, "Переполнение памяти") ;
                SEND_ERROR(text) ;
                 return(-1) ;
                               }
/*---------------------------------------------- Считывание сценария */

                     rewind(file) ;                                 /* Перематываем файл на начало */

       for(row=1, n=0  ; ; row++) {                                 /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Считывание строки */
                     memset(text, 0, sizeof(text)) ;
                  end=fgets(text, sizeof(text)-1, file) ;           /* Считываем очередную строку */
               if(end==NULL)  break ;                               /* Если все считано... */

                  end=strchr(text, '\n') ;                          /* Убираем спец-символы */
               if(end!=NULL)  *end=0 ;
                  end=strchr(text, '\r') ;
               if(end!=NULL)  *end=0 ;

              if(text[0]==';')  continue ;
          
           for(work=text ; *work==' ' || *work=='\t' ; work++) ;

              if(work[0]== 0 )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Разбор на слова */
              memset(words, 0, sizeof(words)) ;

                      i = 0 ;
                words[i]=strtok(work, " \t") ;

          while(words[i]!=NULL && i<30) {
                      i++ ;
                words[i]=strtok(NULL, " \t") ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - -  Обработка меток времени */
            memset(&this->mScenario[n], 0,                          /* Инициализация очередного кадра */
                              sizeof(this->mScenario[n])) ;

         if(memicmp(words[0], "T=",  2) &&
            memicmp(words[0], "DT=", 3)   ) {
                 sprintf(error, "Строка программы сценария должна начинаться с метки времени T=... или DT=... (строка %d)", row) ;
              SEND_ERROR(error) ;
                   return(-1) ;
                                            }

                          end=strchr(words[0], '=') ;
             value=strtod(end+1, &end) ;

         if(*end!=0) {
                          sprintf(error, "Некорректная метка времени (строка %d)", row) ;
                       SEND_ERROR(error) ;
                            return(-1) ;
                     }

         if(!memicmp(words[0], "T=", 2)) {  this->mScenario[n].t        = value ;
                                            this->mScenario[n].sync_type=_T_SYNC ;   }
         else                            {  this->mScenario[n].dt       = value ;
                                            this->mScenario[n].sync_type=_DT_SYNC ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Операция START */
         if(!stricmp(words[1], "START")) {

              if(words[2]==NULL) {
                                    sprintf(error, "Не задано имя объекта (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* Если имя не найдено... */
                           sprintf(error, "Объекта с именем '%s' НЕ существует  (строка %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                 strcpy(desc, words[2]) ;

              if(words[3]!=NULL) {
                        this->mScenario[n].t_par=strtod(words[3], &end) ;
                 if(*end!=0) {
                                  sprintf(error, "Некорректное время активности объекта (строка %d)", row) ;
                               SEND_ERROR(error) ;
                                    return(-1) ;
                             }

                                     strcat(desc, " ") ;
                                     strcat(desc, words[3]) ;
                                 }

                         strcpy(this->mScenario[n].action,  words[1]) ;
                         strcpy(this->mScenario[n].object,  words[2]) ;
                         strcpy(this->mScenario[n].command, desc) ;
                                                n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Операция STOP */
         else
         if(!stricmp(words[1], "STOP")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action, words[1]) ;
                         strcpy(this->mScenario[n].object, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Операция KILL */
         else
         if(!stricmp(words[1], "KILL")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action, words[1]) ;
                         strcpy(this->mScenario[n].object, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Операция EVENT */
         else
         if(!stricmp(words[1], "EVENT")) {

              if(words[2]==NULL) {
                                    sprintf(error, "Не задано имя объекта (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[3]==NULL) {
                                    sprintf(error, "Не задано событие (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* Если имя не найдено... */
                           sprintf(error, "Объекта с именем '%s' НЕ существует  (строка %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                     strcpy(desc, words[2]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[3]) ;

                     strcpy(this->mScenario[n].action,  words[1]) ;
                     strcpy(this->mScenario[n].object,  words[2]) ;
                     strcpy(this->mScenario[n].event,   words[3]) ;
                     strcpy(this->mScenario[n].command, desc) ;
                                            n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Операция SPAWN */
         else
         if(!stricmp(words[1], "SPAWN")) {

              if(words[2]==NULL) {
                                    sprintf(error, "Не задано имя объекта-шаблона (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[3]==NULL) {
                                    sprintf(error, "Не задана размерность залпа (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[4]==NULL) {
                                    sprintf(error, "Не задан темп порождения (строка %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* Если имя не найдено... */
                           sprintf(error, "Объекта с именем '%s' НЕ существует  (строка %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                     strcpy(desc, words[2]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[3]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[4]) ;

                     strcpy(this->mScenario[n].action,  words[1]) ;
                     strcpy(this->mScenario[n].object,  words[2]) ;
                            this->mScenario[n].size  =strtoul(words[3], &end, 10) ;
                            this->mScenario[n].period= strtod(words[4], &end) ;
                     strcpy(this->mScenario[n].command, desc) ;
                                            n++ ;           

                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Операция EXIT */
         else
         if(!stricmp(words[1], "EXEC")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action,  words[1]) ;
                         strcpy(this->mScenario[n].command, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Операция EXIT */
         else
         if(!stricmp(words[1], "EXIT")) {

                         strcpy(this->mScenario[n].action,  words[1]) ;
                                                n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - Неизвестная операция */
         else                            {

                          sprintf(error, "Неизвестная операция (строка %d)", row) ;
                       SEND_ERROR(error) ;
                            return(-1) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }                                 /* CONTINUE.1 */

                  this->mScenario_cnt=n ;

/*--------------------------------------------------- Закрытие файла */

             fclose(file) ;                                         /* Закрываем файла */

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции RUN                     */

  int  RSS_Module_Battle::cRun(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

              RSS_IFace  iface_ ;
                   char *pars[_PARS_MAX] ;
                    int  fast ; 
                 double  time_0 ;              /* Стартовое время расчета */ 
                 double  time_1 ;              /* Текущее время */ 
                 double  time_c ;              /* Абсолютное время расчета */ 
                 double  time_s ;              /* Последнее время отрисовки */ 
                 double  time_w ;              /* Время ожидания */ 
                    int  preObjects_cnt ;
                    int  attempt_cnt ;
                    int  attempt ;
                    int  hit_sum ;
                 double  hit_avg ;
                   char *end ;
                   char  name[1024] ;
                   char  text[1024] ;
       RSS_Objects_List  checked ;
             RSS_Object *clone ;
                  FRAME  frame ;
                    int  n_frame ;
                    int  exit_flag ;
                    int  status ;
                    int  i ;
                    int  j ;

           static  char *callback ;
#define  _CALLBACK_SIZE  128000

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                   fast=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
                              cmd++ ;
                          if(*cmd=='f' || *cmd=='F')  fast=1 ;
                              cmd++ ;
                        }       

       if(*cmd==' ')  cmd++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

                                attempt_cnt=0 ;
            if(pars[0]!=NULL )  attempt_cnt=strtoul(pars[0], &end, 10) ;

            if(attempt_cnt==0)  attempt_cnt=1 ;

/*-------------------------------------------- Подготовка исполнения */

     if(callback==NULL)  callback=(char *)calloc(1, _CALLBACK_SIZE) ;

/*----------------------------------------------------- Цикл попыток */
 
           preObjects_cnt=this->mObjects_cnt ;

                  hit_sum=0 ;

   for(attempt=0 ; attempt<attempt_cnt ; attempt++) {               /* LOOP.0 */

                       RSS_Kernel::battle=1 ;

/*------------------------------------- Инициализация объектов карты */

     for(i=0 ; i<mMapTraces_cnt ; i++) {
                                          mMapTraces[i].points_cnt=0 ;
                                       }
/*--------------------------------- Инициализация контекста объектов */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt


    for(i=0 ; i<OBJECTS_CNT ; i++) {

     if(attempt>0) {

         if(OBJECTS[i]->battle_state==_ACTIVE_STATE) {

                                OBJECTS[i]->vPop() ;
                                OBJECTS[i]->vCalculateShow(0, 0) ;
                                                     }
         if(OBJECTS[i]->battle_state== _SPAWN_STATE) {

                                         OBJECTS[i]->vFree() ;      /* Освобождение ресурсов */
                                 delete  OBJECTS[i] ;

             for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;
                           OBJECTS_CNT-- ;
                                     i-- ;
                                                     }
                   }

           OBJECTS[i]->vResetFeatures(NULL) ;
           OBJECTS[i]->vPrepareFeatures(NULL) ;

                                   }

                       this->kernel->vShow("REFRESH") ;

#undef    OBJECTS
#undef    OBJECTS_CNT

/*----------------------------------------------- Подготовка попытки */

                 this->mObjects_cnt  =preObjects_cnt ;
                 this->mSpawns_cnt   = 0 ;
                 this->mVariables_cnt= 0 ;

      for(i=0 ; i<mObjects_cnt ; i++) {

            mObjects[i].object->vSpecial       ("BATTLE", this) ;
            mObjects[i].object->vResetFeatures (NULL) ;             /* Инициализация свойств объекта */ 
            mObjects[i].object->vCalculateStart(0.) ;
            mObjects[i].active  =1 ;
            mObjects[i].cut_time=0. ;
                                      }

                   exit_flag=0 ;

/*----------------------------------- Подготовка счётчиков поражения */

      RSS_Feature_Hit::hit_count=&this->mHit_cnt ;

                                        mHit_cnt=0 ;

/*------------------------------------ Исполнение начальных операций */

   for(n_frame=0 ; n_frame<mScenario_cnt ; ) {

        status=iFrameExecute(&mScenario[n_frame], 0, 1) ;
/*- - - - - - - - - - - - - - - - - - - Переход к следующему "кадру" */
     if(status==_NEXT_FRAME) {
                                n_frame++ ;  continue ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  Завершение сценария */
     if(status==_EXIT_FRAME) {
                                 exit_flag=1 ;  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - -  Все по времени обработано */
     if(status==_WAIT_FRAME) {
                                  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                             }

           if(exit_flag)  return(-1) ;  

/*-------------------------------------- Главный исполнительный цикл */

                      RSS_Kernel::kernel->next=0 ;

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

  do {
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание... */

           if(this->kernel->next==_RSS_KERNEL_WAIT_STEP) {          /* Управление пошаговым режимом */
                                     Sleep(100) ;
                                      continue ;
                                                         }
           if(this->kernel->next==_RSS_KERNEL_NEXT_STEP) {
                      this->kernel->next=_RSS_KERNEL_WAIT_STEP ;
                                                         }
/*------------------------------------------------ Отработка времени */

              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
          if(fast==0)                                               /* Для реального времени - выдерживаем паузу */
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)

                    hit_avg=((double)(hit_sum+mHit_cnt))/(attempt+1.) ;

      sprintf(text, "Attempt % 5d\r\n" 
                    "   Real % 5.0lf\r\n" 
                    "   Calc % 5.0lf\r\n"
                    "Hit.cur % 5d\r\n"
                    "Hit.avg % 7.1lf\r\n",
                     attempt+1, time_1-time_0, time_c, mHit_cnt, hit_avg) ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*----------------------------------------------- Отработка сценария */

   for( ; n_frame<mScenario_cnt ; ) {                               /* CIRCLE.1 */

        status=iFrameExecute(&mScenario[n_frame], time_c, 1) ;
/*- - - - - - - - - - - - - - - - - - - Переход к следующему "кадру" */
     if(status==_NEXT_FRAME) {
                                n_frame++ ;  continue ;
                             }
/*- - - - - - - - - - - - - - - - - - - Переход к следующему "кадру" */
     if(status==_EXIT_FRAME) {
                                 exit_flag=1 ;  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - -  Все по времени обработано */
     if(status==_WAIT_FRAME) {
                                  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    }                               /* CONTINUE.1 */

           if(exit_flag)  break ;  

/*-------------------------------------------------- Обсчет объектов */

      for(i=0 ; i<mObjects_cnt ; i++) 
        if(mObjects[i].active) {
/*- - - - - - - - - - - - - - - - - - - - Обсчет и отрисовка объекта */
                    memset(callback, 0, sizeof(callback)) ;

            mObjects[i].object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c,
                                                                   callback, _CALLBACK_SIZE-1) ;
            mObjects[i].object->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

         if(mObjects[i].cut_time!=    0. &&
            mObjects[i].cut_time<=time_c   )  mObjects[i].active=0 ;
/*- - - - - - - - - - - - - - - - -  Обработка команд обратной связи */
         if(callback[0]!=0) {

             for(cmd=callback ; *cmd!=0 ; cmd=end+1) {

                end=strchr(cmd, ';') ;
               *end= 0 ;

                      memset(&frame, 0, sizeof(frame)) ;

                if(!memicmp(cmd, "START ", strlen("START "))) {
                     strcpy(frame.action, "START") ;
                     strcpy(frame.object, cmd+strlen("START ")) ;
                                                              }
                else
                if(!memicmp(cmd, "STOP ", strlen("STOP "))) {
                     strcpy(frame.action, "STOP") ;
                     strcpy(frame.object, cmd+strlen("STOP ")) ;
                                                            }
                else
                if(!memicmp(cmd, "KILL ", strlen("KILL "))) {
                     strcpy(frame.action, "KILL") ;
                     strcpy(frame.object, cmd+strlen("KILL ")) ;
                                                            }
                else
                if(!memicmp(cmd, "EXEC ", strlen("EXEC "))) {
                     strcpy(frame.action, "EXEC") ;
                     strcpy(frame.command, cmd+strlen("EXEC ")) ;
                                                            }

                    iFrameExecute(&frame, time_c, 0) ;
                                                     }
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               }
/*------------------------------------------------ Обработка спаунов */

      for(i=0 ; i<mSpawns_cnt ; i++) 
        if(mSpawns[i]!=NULL) {

#define  S  mSpawns[i] 

         while(S->cnt        < S->size &&
               S->next_event < time_c    ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - "Запуск" объекта */
             sprintf(name, "%s_%d", S->object, S->cnt+1) ;

                clone=S->templ->vCopy(name) ;
                clone->battle_state=_SPAWN_STATE ;

                clone->vResetFeatures(NULL) ;                       /* Инициализация свойств объекта */ 

                clone->vCalculateStart(S->next_event) ;
                clone->vCalculate     (S->next_event, time_c, NULL, 0) ;
                clone->vCalculateShow (S->next_event, time_c) ;
/*- - - - - - - - - - - - - - - - - - -  Добавление объекта в список */
           if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("Список объектов сценария слишком велик") ;
                                              exit_flag=1 ;  break ;
                                          }

                   mObjects[mObjects_cnt].object  =clone ;
                   mObjects[mObjects_cnt].active  =  1 ;

           if(S->t_par!=0.)
                   mObjects[mObjects_cnt].cut_time=S->next_event+S->t_par ;
           else    mObjects[mObjects_cnt].cut_time= 0. ;
                            mObjects_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - -  Анализ следующего пуска */
                       S->next_event+=S->period ;
                       S->cnt       ++ ;

                    if(S->cnt==S->size) {  S=NULL ;  break ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                          }
#undef   S
                             }
/*----------------------------------------------- Контроль поражения */

      for(i=0 ; i<mObjects_cnt ; i++)
        if(mObjects[i].active)  mObjects[i].object->vPrepareFeatures(NULL) ;

      for(i=0 ; i<mObjects_cnt ; i++)
        if(mObjects[i].active) {
                                                               checked.Clear() ;
              status=mObjects[i].object->vCheckFeatures(NULL, &checked) ;
           if(status) {

                    mObjects[i].active=mObjects[i].object->vEvent("HIT", time_c) ;

             for(j=0 ; j<checked.List_cnt ; j++)
               if(!stricmp(checked.List[j].relation, "Hit"))
                      checked.List[j].object->vEvent("HITED", time_c) ;

                      }
                               }
/*-------------------------------------------------- Отрисовка сцены */

          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;

       if(this->mMapWindow!=NULL)  SendMessage(this->mMapWindow, WM_PAINT, NULL, NULL) ;

                                                       }
/*-------------------------------------- Главный исполнительный цикл */

     } while(!exit_flag) ;  

                    hit_sum+=mHit_cnt ;
                    hit_avg =((double)hit_sum)/(attempt+1.) ;

              this->kernel->vShow(NULL) ;                           /* Финальная отрисовка */

/*----------------------------------------------------- Цикл попыток */ 
                                                    }               /* END LOOP */

                       RSS_Kernel::battle=0 ;

    if(attempt_cnt==1)  SEND_ERROR("Бой завершен") ;
    else                SEND_ERROR("Моделирование завершено") ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MAP                     */
/*								    */
/*        MAP <Квадрант отображения>                                */

  int  RSS_Module_Battle::cMap(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX  10

      char *pars[_PARS_MAX] ;
       int  quiet_flag ;
       int  trace_oper ;
       int  color_oper ;
  COLORREF  color ;
      char *pos ;
      char  text[1024] ;
      char *end ;
       int  i  ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                   trace_oper=0 ;
                   color_oper=0 ;
                   quiet_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  trace_oper=1 ;
                if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  color_oper=1 ;
                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag=1 ;

                           cmd=end+1 ;
                        }

    if(trace_oper &&
       color_oper   ) {
                        SEND_ERROR("Одновременное использование ключей T и C недопустимо") ;
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
/*--------------------------------------- Задание режима трассировки */

    if(trace_oper) {
/*- - - - - - - - - - - - - - - - - - - - - - -  Определение объекта */
     if(pars[0]==NULL) {
                         SEND_ERROR("Не задано имя объекта. \n"
                                    "Например: MAP/T <Имя_объекта>") ;
                                        return(-1) ;
                       } 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, pars[0]))  break ;

 
    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Операция выполнена, хотя объекта с именем '%s' "
                                         "НЕ существует.", pars[0]) ;
       if(!quiet_flag)  SEND_ERROR(text) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - -  Регистрация трассировки */
    if(mMapTraces_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("Список трассировок объектов слишком велик") ;
                                         return(-1) ;
                                     }

       strncpy(mMapTraces[mMapTraces_cnt].object, pars[0], sizeof(mMapTraces[0].object)-1) ;
                          mMapTraces_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*-------------------------------------------- Задание цвета объекта */

    else
    if(color_oper) {
/*- - - - - - - - - - - - - - - - - - - - - - - -  Определение цвета */
     if(pars[1]==NULL) {
                         SEND_ERROR("Не задан цвет объекта. \n"
                                    "Например: MAP/C <Имя_объекта> GREEN") ;
                                        return(-1) ;
                       } 

              if(!stricmp(pars[1], "RED"  ))  color=RGB(127,   0,   0) ;
         else if(!stricmp(pars[1], "GREEN"))  color=RGB(  0, 127,   0) ;
         else if(!stricmp(pars[1], "BLUE" ))  color=RGB(  0,   0, 127) ;
         else                                {
                         SEND_ERROR("Неизвестное название цвета") ;
                                        return(-1) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  Определение объекта */
     if(pars[0]==NULL) {
                         SEND_ERROR("Не задано имя объекта. \n"
                                    "Например: MAP/C <Имя_объекта> GREEN") ;
                                        return(-1) ;
                       } 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, pars[0]))  break ;

 
    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Операция выполнена, хотя объекта с именем '%s' "
                                         "НЕ существует.", pars[0]) ;
       if(!quiet_flag)  SEND_ERROR(text) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Регистрация цвета */
    if(mMapColors_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("Список цветов объектов слишком велик") ;
                                         return(-1) ;
                                     }

       strncpy(mMapColors[mMapColors_cnt].object, pars[0], sizeof(mMapColors[0].object)-1) ;
               mMapColors[mMapColors_cnt].color =color ;
                          mMapColors_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*----------------------------------------- Создание окна индикатора */

    else           {
                      pos=pars[0] ;   
       if(pos==NULL)  pos="0" ;

          strcpy(Context->action, "MAP") ;
          strcpy(Context->details, pos) ;

     SendMessage(RSS_Kernel::kernel_wnd, 
                 WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                          (LPARAM) Context              ) ;

                   }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*              Реализация инструкций сервисной задачи              */
/*                                                                  */
/*  start  -  Начать отслеживание объекта                           */
/*  stop   -  Прекратить отслеживание объекта                       */
/*  kill   -  Удалить объект                                        */
/*  spawn  -  Порождение серии объектов                             */
/*  event  -  Передать событие на объекта                           */
/*  exec   -  Выполнить системную команду                           */

  int  RSS_Module_Battle::iFrameExecute(FRAME *frame, double  t, int sync_use)

{ 
       static  double  t_last ;
            RSS_IFace  iface ;
           RSS_Object *object ;
                 char  text[1024] ;
                 char *end ;
                  int  status ;
                  int  i ;
                  int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- Инициализация */

/*--------------------------------- Контроль временной синхронизации */

 if(sync_use && frame->sync_type!=_NO_SYNC) {

    if(frame->sync_type==_T_SYNC &&
       frame->t        >= t         )  return(_WAIT_FRAME) ;

    if(frame->sync_type==_DT_SYNC &&
       frame->dt+t_last>=  t        )  return(_WAIT_FRAME) ;

                  t_last=t ;
                                            }
/*---------------------------------------------------- Операция EXIT */

    if(!stricmp(frame->action, "EXIT" )) {

                                     return(_EXIT_FRAME) ;

                                         }
/*---------------------------------------------------- Операция EXIT */
    else
    if(!stricmp(frame->action, "EXEC" )) {

      for(i=0 ; i<RSS_Kernel::kernel->modules_cnt ; i++) {
      
             status=RSS_Kernel::kernel->modules[i].entry->vExecuteCmd(frame->command) ;
          if(status==-1)  return(-1) ;
          if(status== 0)    break ;
                                                         }

          if(status== 1)
             status=RSS_Kernel::kernel->vExecuteCmd(frame->command) ;

          if(status== 1) {
                              sprintf(text, "Неизвестная команда: %s", frame->command) ;
                           SEND_ERROR(text) ;
                               return(_EXIT_FRAME) ;
                         }
                                         }
/*--------------------------------------------------- Операция START */
    else
    if(!stricmp(frame->action, "START")) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* Ищем объект по имени */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* Если имя не найдено... */
                                   sprintf(text, "Объекта '%s' НЕ существует", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;

        for(i=0 ; i<mObjects_cnt ; i++)
          if(object==mObjects[i].object)  break ;
          
           if(i<mObjects_cnt) {                                     /* Если объект уже включен в сценарий... */              
                                    sprintf(text, "Объекта '%s' уже включен в сценарий", frame->object) ;
                                 SEND_ERROR(text) ;
                                     return(_EXIT_FRAME) ;
                              }
/*- - - - - - - - - - - - - - - - - - -  Добавление объекта в список */
     if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("Список объектов сценария слишком велик") ;
                                       return(_EXIT_FRAME) ;
                                    }

         if(object->battle_state!=_SPAWN_STATE) {                   /* Если объект не создан в процессе боя... */
                   object->battle_state=_ACTIVE_STATE ;
                   object->vPush() ;
                                                }

                   object->vSpecial       ("BATTLE", this) ;
                   object->vResetFeatures (NULL) ;
                   object->vCalculateStart(t) ;

               mObjects[mObjects_cnt].object  =object ;
               mObjects[mObjects_cnt].active  =  1 ;

       if(frame->t_par!=0.)
               mObjects[mObjects_cnt].cut_time=t+frame->t_par ;
       else    mObjects[mObjects_cnt].cut_time= 0. ;
                        mObjects_cnt++ ;
/*- - - - - - - - - - - - - - - - - -  Инициализация свойств объекта */
                      object->vResetFeatures(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*---------------------------------------------------- Операция STOP */
    else
    if(!stricmp(frame->action, "STOP")) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* Ищем объект по имени */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* Если имя не найдено... */
                                   sprintf(text, "Объекта '%s' НЕ существует", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;
/*- - - - - - - - - - - - - - - - - - Сброс метки активности объекта */
        for(i=0 ; i<mObjects_cnt ; i++)
          if(object==mObjects[i].object)  break ;

           if(i>=mObjects_cnt) {                                    /* Если объект уже включен в сценарий... */
                                    sprintf(text, "Объекта '%s' не включен в сценарий", frame->object) ;
                                 SEND_ERROR(text) ;
                                     return(_EXIT_FRAME) ;
                              }

               mObjects[i].active=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*---------------------------------------------------- Операция KILL */
    else
    if(!stricmp(frame->action, "KILL")) {
/*- - - - - - - - - - - - - - - - - - -  Определение режима удаления */
                      end=strchr(frame->object, '%') ;
       if(end==NULL)  end=strchr(frame->object, '*') ;
       if(end!=NULL) *end= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Для группового объекта */
       if(end!=NULL) {

            for(i=0, j=0 ; i<mObjects_cnt ; i++) {                       /* Очистка списка боя */

              if(i!=j)  mObjects[j].object=mObjects[i].object ;

              if(mObjects[j].object!=NULL)
               if(memicmp(mObjects[j].object->Name, frame->object,
                                             strlen(frame->object)))  j++ ;
                                                 }

                                          mObjects_cnt=j ;

                                      sprintf(text, "kill/b %s%%", frame->object) ;
              RSS_Kernel::kernel->vExecuteCmd(text) ;               /* Удаление объектов */

                     }
/*- - - - - - - - - - - - - - - - - - - - - - Для одиночного объекта */
       else          {

         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* Ищем объект по имени */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* Если имя не найдено... */
                                   sprintf(text, "Объекта '%s' НЕ существует", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;

         for(i=0 ; i<mObjects_cnt ; i++)                            /* Сброс метки активности объекта */
           if(object==mObjects[i].object)  break ;

           if(i<mObjects_cnt) {
                                 mObjects[i].object=NULL ;
                                 mObjects[i].active=  0 ;
                              }

                                      sprintf(text, "kill/b %s", frame->object) ;
              RSS_Kernel::kernel->vExecuteCmd(text) ;               /* Удаление объекта */
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------------- Операция EVENT */
    else
    if(!stricmp(frame->action, "EVENT")) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* Ищем объект по имени */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* Если имя не найдено... */
                                   sprintf(text, "Объекта '%s' НЕ существует", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Сигнал о событии */
                   object->vEvent(frame->event, t) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------------- Операция SPAWN */
    else
    if(!stricmp(frame->action, "SPAWN")) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* Ищем объект по имени */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* Если имя не найдено... */
                                   sprintf(text, "Объекта '%s' НЕ существует", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      frame->templ=OBJECTS[i] ;
/*- - - - - - - - - - - - - - -  Добавление спауна в список активных */
           for(j=0 ; j<mSpawns_cnt ; j++)
             if(mSpawns[j]==NULL)  break ;

           if(j>=_SPAWNS_MAX) {                                     /* Если мест в списке нет... */
                                   sprintf(text, "Переполнение списка спаун-объектов") ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                mSpawns[j]=frame ;
                           frame->cnt       =0 ;
                           frame->next_event=t ;

             if(j>=mSpawns_cnt)  mSpawns_cnt=j+1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------- Неизвестная операция */

    else                                {

                 sprintf(text, "BATTLE - Неизвестная операция: %s", frame->action) ;
              SEND_ERROR(text) ;
                  return(_EXIT_FRAME) ;
                                        }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(_NEXT_FRAME) ;
}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit_Battle::RSS_Transit_Battle(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Battle::~RSS_Transit_Battle(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Battle::vExecute(void)

{
     HWND  hWnd ;
     RECT  rect ;
      int  x ; 
      int  y ; 
      int  w_x ; 
      int  w_y ; 

/*----------------------------------------------- Создание карты боя */

   if(!stricmp(action, "MAP")) {

         hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                             (LPCDLGTEMPLATE)ProgramModule.Resource("IDD_MAP", RT_DIALOG),
	                        NULL, Task_Battle_Map_dialog, 
                                  (LPARAM)&ProgramModule) ;

             GetWindowRect(hWnd, &rect);
                              w_x=rect.bottom-rect.top +1 ;
                              w_y=rect.right -rect.left+1 ;

                                 x= 0 ;
                                 y= 0 ;
         if(details[0]=='1') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='2') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='3') {   x= RSS_Kernel::display.x-w_x ;
                                 y= 0 ;                                       }
         if(details[0]=='4') {   x= RSS_Kernel::display.x-w_x ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }
         if(details[0]=='5') {   x= RSS_Kernel::display.x-w_x ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='6') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='7') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='8') {   x= 0 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='9') {   x= 0 ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }


                     SetWindowPos(hWnd, NULL, x, y, 0, 0,
                                   SWP_NOOWNERZORDER | SWP_NOSIZE);
                       ShowWindow(hWnd, SW_SHOW) ;

                         SetFocus(ProgramModule.kernel_wnd) ;

                                   ProgramModule.mMapWindow=hWnd ;

                                     }
/*-------------------------------------------------------------------*/

   return(0) ;
}

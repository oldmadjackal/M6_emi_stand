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

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

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
 { "clear",      "c",  "#CLEAR (C) - очистить контекст сценария боя", 
                        NULL,
                       &RSS_Module_Battle::cClear  },
 { "add",        "a",  "#ADD (A) - добавление объекта в сценарий боя", 
                       " ADD <Имя объекта>\n",
                       &RSS_Module_Battle::cAdd    },
 { "list",       "l",  "#LIST (L) - просмотр описания сценароия и состава участников боя", 
                        NULL,
                       &RSS_Module_Battle::cList   },
 { "program",    "p",  "#PROGRAM (P) - загрузка сценария из файла", 
                       " PROGRAM <Имя файла>\n",
                       &RSS_Module_Battle::cProgram },
 { "run",        "r",  "#RUN (R) - запуск исполнения сценария боя", 
                        NULL,
                       &RSS_Module_Battle::cRun },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Battle_instr *RSS_Module_Battle::mInstrList       =NULL ;

                               OBJ  RSS_Module_Battle::mObjects[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mObjects_cnt    =  0 ;

                             FRAME *RSS_Module_Battle::mScenario        =NULL ;
                               int  RSS_Module_Battle::mScenario_cnt    =  0 ;

                               VAR *RSS_Module_Battle::mVariables       =NULL ;
                               int  RSS_Module_Battle::mVariables_cnt   =  0 ;
                              HWND  RSS_Module_Battle::mVariables_Window=NULL ;



/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Battle::RSS_Module_Battle(void)

{
                keyword="EmiStand" ;
         identification="Battle_task" ;

             mInstrList=RSS_Module_Battle_InstrList ;
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
/*------------------------------------------------ Очистка контекста */

           mObjects_cnt=0 ;

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
/*    {T|DT}=<метка времени> EVENT <Имя объекта> <Событие>          */

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
                 sprintf(text, "Строка программы сценария должна начинаться с метки времени T=... или DT=... (строка %d)", row) ;
              SEND_ERROR(text) ;
                   return(-1) ;
                                            }

                          end=strchr(words[0], '=') ;
             value=strtod(end+1, &end) ;

         if(*end!=0) {
                          sprintf(text, "Некорректная метка времени (строка %d)", row) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

         if(!memicmp(words[0], "T=", 2)) {  this->mScenario[n].t     =value ;
                                            this->mScenario[n].dt_use=   0 ;  }
         else                            {  this->mScenario[n].dt    =value ;
                                            this->mScenario[n].dt_use=   1 ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Операция START */
         if(!stricmp(words[1], "START")) {

              if(words[2]==NULL) {
                                    sprintf(text, "Не задано имя объекта (строка %d)", row) ;
                                 SEND_ERROR(text) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' НЕ существует  (строка %d)", words[2], row) ;
                        SEND_ERROR(text) ;
                            return(-1) ;
                                 }

                 strcpy(desc, words[2]) ;

              if(words[3]!=NULL) {
                        this->mScenario[n].t_par=strtod(words[3], &end) ;
                 if(*end!=0) {
                                  sprintf(text, "Некорректное время активности объекта (строка %d)", row) ;
                               SEND_ERROR(text) ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Операция EVENT */
         else
         if(!stricmp(words[1], "EVENT")) {

              if(words[2]==NULL) {
                                    sprintf(text, "Не задано имя объекта (строка %d)", row) ;
                                 SEND_ERROR(text) ;
                                      return(-1) ;
                                 }

              if(words[3]==NULL) {
                                    sprintf(text, "Не задано событие (строка %d)", row) ;
                                 SEND_ERROR(text) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' НЕ существует  (строка %d)", words[2], row) ;
                        SEND_ERROR(text) ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Операция EXIT */
         else
         if(!stricmp(words[1], "EXIT")) {

                         strcpy(this->mScenario[n].action,  words[1]) ;
                                                n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - Неизвестная операция */
         else                            {

                          sprintf(text, "Неизвестная операция (строка %d)", row) ;
                       SEND_ERROR(text) ;
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
     double  time_0 ;              /* Стартовое время расчета */ 
     double  time_1 ;              /* Текущее время */ 
     double  time_c ;              /* Абсолютное время расчета */ 
     double  time_s ;              /* Последнее время отрисовки */ 
     double  time_w ;              /* Время ожидания */ 
       char *end ;
       char  text[1024] ;
        int  n_frame ;
        int  exit_flag ;
        int  status ;
        int  i ;

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

/*-------------------------------------------- Подготовка исполнения */

                 this->mVariables_cnt=0 ;

      for(i=0 ; i<mObjects_cnt ; i++) {

            mObjects[i].object->vSpecial       ("KERNEL", this->kernel) ;
            mObjects[i].object->vCalculateStart() ;
            mObjects[i].active  =1 ;
            mObjects[i].cut_time=0. ;
                                      }

                   exit_flag=0 ;

/*------------------------------------ Исполнение начальных операций */

   for(n_frame=0 ; n_frame<mScenario_cnt ; ) {

        status=iFrameExecute(&mScenario[n_frame], 0) ;
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

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

  do {
           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание... */

/*------------------------------------------------ Отработка времени */

              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

              time_w=time_c-(time_1-time_0) ;

           if(time_w>=0)  Sleep(time_w*1000) ;

      sprintf(text, "Real % 5.0lf\r\n" 
                    "Calc % 5.0lf\r\n",
                     time_1-time_0, time_c) ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*----------------------------------------------- Отработка сценария */

   for( ; n_frame<mScenario_cnt ; ) {                               /* CIRCLE.1 */

        status=iFrameExecute(&mScenario[n_frame], time_c) ;
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

            mObjects[i].object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c) ;
            mObjects[i].object->vCalculateShow() ;

         if(mObjects[i].cut_time!=    0. &&
            mObjects[i].cut_time<=time_c   )  mObjects[i].active=0 ;
                               }
/*-------------------------------------------------- Отрисовка сцены */

          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*-------------------------------------- Главный исполнительный цикл */

     } while(!exit_flag) ;  

              this->kernel->vShow(NULL) ;                           /* Финальная отрисовка */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*              Реализация инструкций сервисной задачи              */
/*                                                                  */
/*  start  -  Начать отслеживание объекта                           */

  int  RSS_Module_Battle::iFrameExecute(FRAME *frame, double  t)

{ 
       static  double  t_last ;
            RSS_IFace  iface ;
           RSS_Object *object ;
                 char  text[1024] ;
                  int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- Инициализация */

/*----------------------------------------- Контроль временной метки */

    if(frame->dt_use==0 &&
       frame->t     >=t   )  return(_WAIT_FRAME) ;

    if(frame->dt_use   ==1 &&
       frame->dt+t_last>=t   )  return(_WAIT_FRAME) ;

/*---------------------------------------------------- Операция EXIT */

    if(!stricmp(frame->action, "EXIT" )) {

                                 SEND_ERROR("Бой завершен") ;
                                     return(_EXIT_FRAME) ;

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

                   object->vSpecial       ("KERNEL", this->kernel) ;
                   object->vCalculateStart() ;

               mObjects[mObjects_cnt].object  =object ;
               mObjects[mObjects_cnt].active  =  1 ;

       if(frame->t_par!=0.)
               mObjects[mObjects_cnt].cut_time=t+frame->t_par ;
       else    mObjects[mObjects_cnt].cut_time= 0. ;
                        mObjects_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------------- Операция START */
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
/*--------------------------------------------- Неизвестная операция */

    else                                {

                 sprintf(text, "BATTLE - Неизвестная операция: %s", frame->action) ;
              SEND_ERROR(text) ;
                  return(_EXIT_FRAME) ;
                                        }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

       t_last=t ;

   return(_NEXT_FRAME) ;
}


/********************************************************************/
/*								    */
/*            Определение нужного вычислителя                       */

  RSS_Kernel *RSS_Module_Battle::iGetCalculator(void)

{
  int  status ;
  int  i ;

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("STD_EXPRESSION", NULL, NULL, NULL, 
                                                          NULL, NULL, NULL ) ;
         if(status==0)  return(CALC[i]) ;
                                     }

#undef   CALC_CNT
#undef   CALC

   return(NULL) ;
}


/********************************************************************/
/*								    */
/*                   Вывод числа в бинарном виде                    */

  void  RSS_Module_Battle::iGetBinary(char *text, int  digits, int  value)

{
   int  i ;
     
       memset(text, 0, digits+1) ;

  for(i=0 ; i<digits ; i++) 
    if((1<<i) & value)  text[digits-i-1]='1' ;
    else                text[digits-i-1]='0' ;
}

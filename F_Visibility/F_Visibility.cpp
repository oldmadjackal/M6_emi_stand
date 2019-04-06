/*********************************************************************/
/*								     */
/*	МОДУЛЬ УПРАВЛЕНИЯ ОПРЕДЕЛЕНИЕМ ПОРАЖЕНИЯ ОБЪЕКТОВ            */
/*								     */
/*********************************************************************/


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

#include "F_Visibility.h"

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


/*********************************************************************/
/*								     */
/*		    	Программный модуль                           */

     static   RSS_Module_Visibility  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	Идентификационный вход                       */

 F_VISIBILITY_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_VISIBILITY_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**               ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ                 **/
/**	           ОБНАРУЖЕНИЕМ ПЕРЕСЕЧЕНИЯ ОБЪЕКТОВ	            **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            Список команд                          */

  struct RSS_Module_Visibility_instr  RSS_Module_Visibility_InstrList[]={

 { "help",     "?",  "#HELP   - список доступных команд", 
                      NULL,
                     &RSS_Module_Visibility::cHelp       },
 { "add",      "a",  "#ADD (A) - добавление точки наблюдения ", 
                     " ADD <Имя объекта> <Имя точки> <X-base> <Z-base> <Z-base>\n"
                     "   зарегистрировать точку наблюдение с заданными координатами относительно базы объекта\n",
                     &RSS_Module_Visibility::cAdd      },
 { "view",     "v",  "#VIEW (V) - поле зрения точки наблюдения ", 
                     " VIEW <Имя объекта> <Имя точки> <Ширина> <Высота> <Глубина>\n"
                     "   задать угловые размеры и глубину поля зрения точки наблюдения\n",
                     &RSS_Module_Visibility::cView     },
 {  NULL }
                                                              } ;

/*********************************************************************/
/*								     */
/*		     Общие члены класса             		     */

    struct RSS_Module_Visibility_instr *RSS_Module_Visibility::mInstrList=NULL ;


/*********************************************************************/
/*								     */
/*		       Конструктор класса			     */

     RSS_Module_Visibility::RSS_Module_Visibility(void)

{
	   keyword="EmiStand" ;
    identification="Visibility" ;
          category="Feature" ;

        mInstrList=RSS_Module_Visibility_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса			     */

    RSS_Module_Visibility::~RSS_Module_Visibility(void)

{
}


/*********************************************************************/
/* 								     */
/*		        Стартовая разводка                           */

    void  RSS_Module_Visibility::vStart(void)

{
/*-------------------------------------------- Регистрируем свойство */

   feature_modules=(RSS_Kernel **)
                     realloc(feature_modules, 
                              (feature_modules_cnt+1)*sizeof(feature_modules[0])) ;

      feature_modules[feature_modules_cnt]=&ProgramModule ;
                      feature_modules_cnt++ ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*		        Создать свойство                	     */

    RSS_Feature *RSS_Module_Visibility::vCreateFeature(RSS_Object *object, RSS_Feature *feature_ext)

{
  RSS_Feature *feature ;


    if(feature_ext!=NULL)  feature= feature_ext ;
    else                   feature=new RSS_Feature_Visibility ;

         feature->Object=object ;

  return(feature) ;
}


/*********************************************************************/
/*								     */
/*		        Выполнить команду       		     */

  int  RSS_Module_Visibility::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "VISIBILITY"
#define  _SECTION_SHRT_NAME   "VIS"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Visibility features:") ;
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
       if(status)  SEND_ERROR("Секция VISIBILITY: Неизвестная команда") ;
                                            return(0) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Visibility::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE VISIBILITY\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef   MARKS_LIST
#undef   MARKS_LIST_CNT

}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Visibility::vReadSave(std::string *data)

{
                    char *buff ;
                     int  buff_size ;
             std::string  decl ;
                    char *work ;
                    char  text[1024] ;
                    char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE VISIBILITY\n", 
                      strlen("#BEGIN MODULE VISIBILITY\n")))  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- Построчная разборка */

                work=buff+strlen("#BEGIN MODULE VISIBILITY\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------------- ??????? */

          if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- Неизвестный спецификатор */

     else                                                {

                sprintf(text, "Module VISIBILITY: "
                              "Неизвестная спецификация %20.20s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- Построчная разборка */
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции Help                    */

  int  RSS_Module_Visibility::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Visibility_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ADD                     */
/*								    */
/*  ADD <Имя объекта> <Имя точки> <X-base> <Z-base> <Z-base>        */

  int  RSS_Module_Visibility::cAdd(char *cmd)

{
#define  _PARS_MAX  10

                RSS_Object *object ;
    RSS_Feature_Visibility *vis ; 
                      char *pars[_PARS_MAX] ;
                      char *object_name ;
                      char *point_name ;
                    double  x_base, y_base, z_base ;
                      char *end ;
                      char  text[1024] ;
                       int  n ;
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

              object_name=pars[0] ;
               point_name=pars[1] ;

/*-------------------------------------------- Извлечение параметров */

          x_base=strtod(pars[2], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат параметра X-base") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          y_base=strtod(pars[3], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат параметра Y-base") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          z_base=strtod(pars[4], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат параметра Z-base") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*---------------------------------------------------- Поиск объекта */

        object=FindObject(object_name, &vis) ;
     if(object==NULL)  return(-1) ;

/*------------------------------------------- Поиск точки наблюдения */

    for(n=0 ; n<vis->Observers_cnt ; n++)
      if(!stricmp(vis->Observers[n].name, point_name))  break ;

/*---------------------------------------- Создание точки наблюдения */

    if(n==vis->Observers_cnt) {

         vis->Observers_cnt++ ;

         vis->Observers=(RSS_Feature_Visibility_Observer *)
                          realloc(vis->Observers, vis->Observers_cnt*sizeof(vis->Observers[0])) ;

         memset(&vis->Observers[n], 0, sizeof(vis->Observers[n]));

                              }
/*------------------------------------------- Регистрация параметров */

   strncpy(vis->Observers[n].name, point_name, sizeof(vis->Observers[n])-1) ;
           vis->Observers[n].x_base=x_base ;
           vis->Observers[n].y_base=y_base ;
           vis->Observers[n].z_base=z_base ;

/*-------------------------------------------------------------------*/

#undef  _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции VIEW                    */
/*								    */
/*  VIEW <Имя объекта> <Имя точки> <Ширина> <Высота> <Глубина>      */

  int  RSS_Module_Visibility::cView(char *cmd)

{
#define  _PARS_MAX  10

                RSS_Object *object ;
    RSS_Feature_Visibility *vis ; 
                      char *pars[_PARS_MAX] ;
                      char *object_name ;
                      char *point_name ;
                    double  a_view, e_view, range ;
                      char *end ;
                      char  text[1024] ;
                       int  n ;
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

              object_name=pars[0] ;
               point_name=pars[1] ;

/*-------------------------------------------- Извлечение параметров */

          a_view=strtod(pars[2], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат ширины поля зрения") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          e_view=strtod(pars[3], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат высоты поля зрения") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          range=strtod(pars[4], &end) ;
      if(*end!=0) {
                        sprintf(text, "Некорректный формат глубины поля зрения") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*---------------------------------------------------- Поиск объекта */

        object=FindObject(object_name, &vis) ;
     if(object==NULL)  return(-1) ;

/*------------------------------------------- Поиск точки наблюдения */

    for(n=0 ; n<vis->Observers_cnt ; n++)
      if(!stricmp(vis->Observers[n].name, point_name))  break ;

    if(n==vis->Observers_cnt) {
                                 sprintf(text, "Неизвестная точка наблюдения") ;
                              SEND_ERROR(text) ;
                                  return(-1) ;
                              }
/*------------------------------------------- Регистрация параметров */

           vis->Observers[n].v_azim=a_view ;
           vis->Observers[n].v_elev=e_view ;
           vis->Observers[n].range =range ;

/*-------------------------------------------------------------------*/

#undef  _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*                       Поиск обьекта по имени                     */

  RSS_Object *RSS_Module_Visibility::FindObject(char *name, RSS_Feature_Visibility **vis)

{
  RSS_Object *object ;
        char  text[1024] ;
         int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- Поиск объекта по имени */ 

               object=NULL ;

   for(i=0 ; i<OBJECTS_CNT ; i++)                                   /* Ищем объект по имени */
     if(!stricmp(OBJECTS[i]->Name, name)) {
                                              object=OBJECTS[i] ;
                                                 break ;
                                          }

    if(object==NULL) {                                              /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                     }
/*---------------------------------------------- Извлечение свойства */

               *vis=NULL ;

      for(i=0 ; i<object->Features_cnt ; i++)                       /* Извлекаем ссылки на свойства */
        if(!stricmp(object->Features[i]->Type, "Visibility")) {
               *vis=(RSS_Feature_Visibility *)object->Features[i] ;
                                    break ;
                                                              }                                                   

    if(*vis==NULL) {                                                /* Если свойство не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ поддерживает свойство Visibility", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                   }
/*-------------------------------------------------------------------*/ 

   return(object) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**							           **/
/**               ОПИСАНИЕ КЛАССА ВИДИМОСТИ ОБЪЕКТА                **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Статические переменные			    */

         int *RSS_Feature_Visibility::hit_count=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Feature_Visibility::RSS_Feature_Visibility(void)

{
   strcpy(Type, "Visibility") ;

    Context         =new RSS_Transit_Visibility ;
    Context->feature=this ;

    Observers    =NULL ;
    Observers_cnt=  0 ;

       Bodies    =NULL ;
       Bodies_cnt=  0 ;

      Sector_color=  0 ;  
      Sector_dlist=  0 ;  

}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Feature_Visibility::~RSS_Feature_Visibility(void)

{
   delete Context ;
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Feature_Visibility::vReadSave(       char *title,  
                                            std::string *data, 
                                                   char *reference)

{
  RSS_Kernel *Calc ;
         int  body_num ;
        char *buff ;
         int  buff_size ;
        char *work ;
        char *decl ;
        char *end ;
        char *sep ;
        char *coord ;
         int  num ;
        char  error[1024] ;
        char  text[1024] ;
         int  status ;
         int  i ;

#define    VERTEXES      Bodies[body_num].Vertexes
#define    VERTEXES_CNT  Bodies[body_num].Vertexes_cnt
#define    FACETS        Bodies[body_num].Facets
#define    FACETS_CNT    Bodies[body_num].Facets_cnt


/*----------------------------------------------- Контроль заголовка */

       if(stricmp(title, "FEATURE VISIBILITY"))  return ;

/*---------------------------------- Удаление предыдущей версии тела */

                    vBodyDelete(reference) ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- Построчная разборка */

//                     Bodies_cnt= 0 ;
                         body_num=Bodies_cnt-1 ;

    for(work=buff ; ; work=end+1) {                                 /* CIRCLE.0 */

           end=strchr(work, '\n') ;
        if(end==NULL)  break ;
          *end=0 ;

/*------------------------------------------------------------- Тело */

          if(!memicmp(work, "$BODY", strlen("$BODY"))) {

                    work+=strlen("$BODY") ;

                            body_num++ ;
                          Bodies_cnt++ ;

               Bodies=(RSS_Feature_Visibility_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE VISIBILITY: "
                                "Недостаточно памяти для списка тел\n") ;
               SEND_ERROR(text) ;
                                        break ;
                             }

                 memset(&Bodies[body_num], 0, sizeof(Bodies[0])) ;

                    for( ; *work==' ' || *work=='\t' ; work++) ;

            if(reference!=NULL)                                         /* Заносим имя тела */ 
                     strcpy(Bodies[body_num].name, reference) ;
            else     strcpy(Bodies[body_num].name, work) ;

                            Bodies[body_num].extrn_pars=Pars_work ;
                            Bodies[body_num].recalc    = 1 ;

                                                       }
/*---------------------------------------------------------- Вершина */

     else if(!memicmp(work, "$VERTEX", strlen("$VERTEX"))) {

           if(body_num==-1)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - Выделение номера вершины */
               decl=work+strlen("$VERTEX") ;
                num=strtol(decl, &decl, 10) ;

           if(num<0 || *decl!='=') {
                sprintf(text, "Section FEATURE VISIBILITY: "
                              "Неверный формат спецификатора VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  Расширение списка точек */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Visibility_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE VISIBILITY: Недостаточно памяти") ;
                                   break ;
                               }                    

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - Выделение описателей X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* Выделяем память под описание */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE VISIBILITY: Недостаточно памяти") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE VISIBILITY: "
                                "Неверный формат спецификатора VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE VISIBILITY: "
                                "Неверный формат спецификатора VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                               *sep= 0 ;
                            VERTEXES[num].y_formula=sep+1 ;
/*- - - - - - - - - - - - - - - - - - - - -  Определение вычислителя */
             Calc=iGetCalculator() ;
          if(Calc== NULL) {
               SEND_ERROR("Section FEATURE SHOW: Не загружен вычислитель стандартных выражений") ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - Расчет координат точек */
                                          VERTEXES[num].x_calculate=NULL ;
                                          VERTEXES[num].y_calculate=NULL ;
                                          VERTEXES[num].z_calculate=NULL ;

      do {
              coord="X" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].x_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].x,
                                     &VERTEXES[num].x_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* Если есть ошибки... */
                      
              coord="Y" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].y_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].y,
                                     &VERTEXES[num].y_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* Если есть ошибки... */

              coord="Z" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].z_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].z,
                                     &VERTEXES[num].z_calculate,
                                            error) ;
         } while(0) ;

          if(status==-1) {
                  sprintf(text, "Section FEATURE VISIBILITY: Ошибка вычислителя\n\n"
                                "Вершина %d, координата %s\n"
                                "%s" ,
                                 num, coord, error) ;
               SEND_ERROR(text) ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }
/*------------------------------------------------------------ Грань */
  
     else if(!memicmp(work, "$FACET", strlen("$FACET"))) {

           if(body_num==-1)  continue ; ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Проход на описание */
              decl=strchr(work, '=') ;
           if(decl==NULL) {
                sprintf(text, "Section FEATURE VISIBILITY: "
                              "Неверный формат спецификатора FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - Добавление новой грани */
              FACETS=(RSS_Feature_Visibility_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE VISIBILITY: Недостаточно памяти") ;
                                break ;
                            }

                memset(&FACETS[FACETS_CNT], 0, sizeof(FACETS[0])) ;

                               FACETS_CNT++ ;
/*- - - - - - - - - - - - - - - - - - - - Формирование списка вершин */
#define  F    FACETS[FACETS_CNT-1]

          for(i=0 ; i<_VERTEX_PER_FACET_MAX ; i++) {

                    F.vertexes[i]=strtol(decl, &decl, 10) ;

               if(*decl==0)  break ;
                   decl++ ;
                                                   }

              if(i==_VERTEX_PER_FACET_MAX) {
                  SEND_ERROR("Section FEATURE VISIBILITY: Слишком много вершин у грани") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- Неизвестный спецификатор */

     else                                                {

                sprintf(text, "Section FEATURE VISIBILITY: "
                              "Неизвестная спецификация %s10.10", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- Построчная разборка */

                                  }                                 /* CONTINUE.0 */

/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/

#undef    VERTEXES
#undef    VERTEXES_CNT
#undef    FACETS
#undef    FACETS_CNT

}


/********************************************************************/
/*								    */
/*                     Выдать информацию о свойстве   		    */

    void RSS_Feature_Visibility::vGetInfo(std::string *text)

{
  int  i ;


     *text="Visibility: " ;

  for(i=0 ; i<this->Observers_cnt ; i++) {

      if(i!=0)  text->append("            ") ;
                text->append(Observers[i].name) ;
                text->append("\r\n") ;
                                         }
}


/*********************************************************************/
/*								     */
/*                          Удалить тело                             */

    void RSS_Feature_Visibility::vBodyDelete(char *body)
{
   int  cnt ;
   int  n_body ;

/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {                                                /* Удаление всех тел */

        for(cnt=this->Bodies_cnt ; cnt>0 ; cnt--)
                         this->vBodyDelete(this->Bodies[0].name) ;

                              return ;
                   }

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;

           if(n_body==this->Bodies_cnt)  return ;

/*---------------------------------------------------- Удаление тела */

       if(this->Bodies[n_body].Vertexes_cnt)                        /* Освобождаем список вершин */
            free(this->Bodies[n_body].Vertexes) ;

       if(this->Bodies[n_body].Facets_cnt)                          /* Освобождаем список граней */
            free(this->Bodies[n_body].Facets) ;

       if(this->Bodies_cnt>1) {                                     /* Если более одного тела - */
                                                                    /*   - поджимаем список тел */
         if(n_body!=this->Bodies_cnt-1) 
               memmove(&this->Bodies[n_body], 
                       &this->Bodies[n_body+1],
                          sizeof(this->Bodies[0])*(this->Bodies_cnt-n_body-1)) ;
                              }
       else                   {                                     /* Если одно только тело -  */
                                  free(this->Bodies) ;              /*   - удаляем его          */
                                       this->Bodies=NULL ;
                              }

                                    this->Bodies_cnt-- ;            /* Корректируем счетчик тел */

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*                   Задание базовой точки тела                     */

    void RSS_Feature_Visibility::vBodyBasePoint(  char *body,                   
                                         double  x, 
                                         double  y, 
                                         double  z )
{
   int  n_body ;

/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- Занесение параметров */

       Bodies[n_body].x_base     =x ;
       Bodies[n_body].y_base     =y ;
       Bodies[n_body].z_base     =z ;

       Bodies[n_body].Matrix_flag=0 ;
       Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                     Задание ориентации тела                      */

    void RSS_Feature_Visibility::vBodyAngles(  char *body,                   
                                      double  azim, 
                                      double  elev, 
                                      double  roll    )
{
   int  n_body ;


/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- Занесение параметров */

       Bodies[n_body].a_azim     =azim ;
       Bodies[n_body].a_elev     =elev ;
       Bodies[n_body].a_roll     =roll ;

       Bodies[n_body].Matrix_flag=  0 ;
       Bodies[n_body].recalc     =  1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*               Задание матрицы положения тела                     */

    void RSS_Feature_Visibility::vBodyMatrix(  char *body,
                                      double  matrix[4][4] )
{
   int  n_body ;

/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- Занесение параметров */

       memcpy(Bodies[n_body].Matrix, matrix, sizeof(Bodies[n_body].Matrix)) ;

              Bodies[n_body].Matrix_flag=1 ;
              Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*           Задание смещения положения и ориентации тела           */

    void RSS_Feature_Visibility::vBodyShifts(   char *body,                   
                                       double  x, 
                                       double  y, 
                                       double  z, 
                                       double  azim, 
                                       double  elev, 
                                       double  roll    )
{
   int  n_body ;


/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- Занесение параметров */

       Bodies[n_body].x_base_s=x ;
       Bodies[n_body].y_base_s=y ;
       Bodies[n_body].z_base_s=z ;
       Bodies[n_body].a_azim_s=azim ;
       Bodies[n_body].a_elev_s=elev ;
       Bodies[n_body].a_roll_s=roll ;

       Bodies[n_body].recalc  =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*              Задание списка параметров тела                      */

    void RSS_Feature_Visibility::vBodyPars(char *body, 
                                    struct RSS_Parameter *pars)
{
   int  n_body ;

/*-------------------------------------- Идентификация тела по имени */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }
/*--------------------------------------------- Занесение параметров */

                                                Pars_work=pars ;

   if(n_body<this->Bodies_cnt)  Bodies[n_body].extrn_pars=pars ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            Определение нужного вычислителя                       */

  RSS_Kernel *RSS_Feature_Visibility::iGetCalculator(void)

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
/*       Сброс контекста проверки непротиворечивости свойства       */  

    int RSS_Feature_Visibility::vResetCheck(void *data)

{
           hit_done=0 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*       Подготовка к проверке непротиворечивости свойства          */  

    int RSS_Feature_Visibility::vPreCheck(void *data)

{

//      if(this->any_weapon==0 &&                                     /* Если объект не будет проверяться по Hit-свойству */
//         this->any_target==0 &&
//         this->target[0] ==0   )  return(0) ;

//      if(track_s.mark)  track_s_prv=track_s ;

         this->RecalcPoints() ;

//              track_s.mark=1 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Проверка непротиворечивости свойства              */  

    int RSS_Feature_Visibility::vCheck(void *data, RSS_Objects_List *checked)

{
 RSS_Object *object ;
        int  n ;            /* Индекс объекта в списке объектов*/
        int  m ;            /* Индекс свойства "Пересечение" в списке свойств */
        int  hit_flag ;
        int  i ;

#define  OBJECTS_CNT                     RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                         RSS_Kernel::kernel_objects
#define  VIS         ((RSS_Feature_Visibility *)object->Features[m])

/*------------------------------------------------ Входной контроль  */

//      if(this->any_weapon==0 &&                                     /* Если цель не назначена... */
//         this->target[0] ==0   )  return(0) ;

/*------------------------------------------------- Обсчет поражения */

                                      hit_flag=0 ;

   for(n=0 ; n<OBJECTS_CNT ; n++) {                                 /* CIRCLE.1 - Перебор объектов */

           object=OBJECTS[n] ;

//     if(this->target[0]!=0)                                         /* Если пересечение только с заданной целью... */
//      if(strcmp(object->Name, this->target))  continue ;            

    for(m=0 ; m<object->Features_cnt ; m++)                         /* Проверяем, связано ли с данным объектом         */
      if(!strcmp(object->Features[m]->Type, this->Type ))  break ;  /*  свойство "Пересечение", если нет - обходим его */

      if(m==object->Features_cnt)  continue ;                       /* Если объект-цель не обладает своиством Visibility... */

     if(VIS->Bodies_cnt==0)  continue ;                             /* Если свойство Visibility - пустое */

//     if(this->target[0]==0)                                         /* Если пересечение с произвольной целью и это не цель */
//      if(VIS->any_target==0)  continue ;
     
      if(!iOverallTest(VIS))  continue ;                            /* Если габариты объектов не пересекаются... */
/*- - - - - - - - - - - - - - - - - - - - - - Контроль поражения тел */
     for(i=0 ; i< VIS->Bodies_cnt ; i++) {                          /* CIRCLE.2 - Перебор тел цели  */

//       if(!iFacetsTest(VIS->Bodies[i] ))  continue;     

         checked->Add(object, this->Type) ;                         /* Регистрируем CHECK-связь */

       if(!VIS->hit_done)                                           /* Модифицируем общий счётчик поражения */
        if(hit_count!=NULL) (*hit_count)++ ;

                          VIS->hit_done=1 ; 

                       hit_flag=1 ;
                          break ;
                                         }                          /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                          break ;
                                  }                                 /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef   VIS

   return(hit_flag) ;
}


/********************************************************************/
/*								    */
/*                    Отображение данных по свойству                */  

    int RSS_Feature_Visibility::vShow(void *data)

{
       strcpy(Context->action, "SHOW") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

   return(0) ;
}


    void  RSS_Feature_Visibility::iShow(void)

{
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
   RSS_Point  points[5] ;
         int  status ;
         int  i ;
         int  j ;

#define  BS  points[0]
#define  LU  points[1]
#define  LD  points[2]
#define  RU  points[3]
#define  RD  points[4]

/*------------------------------------------------- Входной контроль */

     if(this->Observers_cnt==0)  return ;

/*-------------------------------- Резервирование дисплейного списка */

     if(Sector_dlist==0) {
           Sector_dlist=RSS_Kernel::display.GetList(2) ;
                         }

     if(Sector_dlist==0)  return ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- Настройка контекста отображения */

             glNewList(Sector_dlist, GL_COMPILE) ;                  /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
              glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */

/*------------------------------------ Отрисовка секторов наблюдения */

      for(i=0 ; i<Observers_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - - -  Рассчет границы сектора */
             BS.x= 0. ;
             BS.y= 0. ;
             BS.z= 0. ;
             LU.x=-Observers[i].range*sin(Observers[i].v_azim/2.*_GRD_TO_RAD) ;
             LU.y= Observers[i].range*sin(Observers[i].v_elev/2.*_GRD_TO_RAD) ;
             LU.z= Observers[i].range ;
             LD.x= LU.x ;
             LD.y=-LU.y ;
             LD.z= LU.z ;
             RU.x=-LU.x ;
             RU.y= LU.y ;
             RU.z= LU.z ;
             RD.x=-LU.x ;
             RD.y=-LU.y ;
             RD.z= LU.z ;
/*- - - - - - - - - - - - - - - - - - Рассчет матрицы преобразования */
                Summary.LoadE      (4, 4) ;
                  Local.Load4d_base(Observers[i].x_base, 
                                    Observers[i].y_base, 
                                    Observers[i].z_base ) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-this->Object->a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-this->Object->a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-this->Object->a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;
/*- - - - - - - - - - - - - - - - - - - - - - - Перерасчет координат */
         for(j=0 ; j<5 ; j++) {

                          Point.LoadZero(4, 1) ;
                          Point.SetCell (0, 0, points[j].x) ;
                          Point.SetCell (1, 0, points[j].y) ;
                          Point.SetCell (2, 0, points[j].z) ;
                          Point.SetCell (3, 0,  1) ;
                          Point.LoadMul (&Summary, &Point) ;
              points[j].x=Point.GetCell (0, 0)+this->Object->x_base ;
              points[j].y=Point.GetCell (1, 0)+this->Object->y_base ;
              points[j].z=Point.GetCell (2, 0)+this->Object->z_base ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - Отрисовка границ сектора */
             glColor4d(GetRValue(Sector_color)/256., 
                       GetGValue(Sector_color)/256.,
                       GetBValue(Sector_color)/256., 0.2) ;

               glBegin(GL_TRIANGLE_FAN) ;

            glVertex3d(BS.x, BS.y, BS.z) ;
            glVertex3d(LU.x, LU.y, LU.z) ;
            glVertex3d(LD.x, LD.y, LD.z) ;
            glVertex3d(RD.x, RD.y, RD.z) ;
            glVertex3d(RU.x, RU.y, RU.z) ;
            glVertex3d(LU.x, LU.y, LU.z) ;

                 glEnd() ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }

/*----------------------------- Восстановление контекста отображения */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*                    Проверка пересечения габаритов                */ 

    int RSS_Feature_Visibility::iOverallTest(RSS_Feature_Visibility *trgt)
{
/*
   double  s_min, s_max ;
// double  d_min, d_max ;
   double  t_min, t_max ;

                                               s_min =this->track_s    .x ;
                                               s_max =this->track_s_prv.x ;
  if(this->track_s.x > this->track_s_prv.x) {  s_min =this->track_s_prv.x ;
                                               s_max =this->track_s    .x ;  }

                                               t_min =trgt->track_s    .x ;
                                               t_max =trgt->track_s_prv.x ;
  if(trgt->track_s.x > this->track_s_prv.x) {  t_min =trgt->track_s_prv.x ;
                                               t_max =trgt->track_s    .x ;  }

                                               t_min+=(trgt->overall.x_min-trgt->track_s.x) ;
                                               t_max+=(trgt->overall.x_max-trgt->track_s.x) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     

                                               s_min =this->track_s    .y ;
                                               s_max =this->track_s_prv.y ;
  if(this->track_s.y > this->track_s_prv.y) {  s_min =this->track_s_prv.y ;
                                               s_max =this->track_s    .y ;  }

                                               t_min =trgt->track_s    .y ;
                                               t_max =trgt->track_s_prv.y ;
  if(trgt->track_s.y > this->track_s_prv.y) {  t_min =trgt->track_s_prv.y ;
                                               t_max =trgt->track_s    .y ;  }

                                               t_min+=(trgt->overall.y_min-trgt->track_s.y) ;
                                               t_max+=(trgt->overall.y_max-trgt->track_s.y) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     

                                               s_min =this->track_s    .z ;
                                               s_max =this->track_s_prv.z ;
  if(this->track_s.z > this->track_s_prv.z) {  s_min =this->track_s_prv.z ;
                                               s_max =this->track_s    .z ;  }

                                               t_min =trgt->track_s    .z ;
                                               t_max =trgt->track_s_prv.z ;
  if(trgt->track_s.z > this->track_s_prv.z) {  t_min =trgt->track_s_prv.z ;
                                               t_max =trgt->track_s    .z ;  }

                                               t_min+=(trgt->overall.z_min-trgt->track_s.z) ;
                                               t_max+=(trgt->overall.z_max-trgt->track_s.z) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     
*/
     return(1) ;
}


/********************************************************************/
/*								    */
/*                Проверка пересечения граней тела цели             */ 

    int RSS_Feature_Visibility::iFacetsTest(RSS_Feature_Visibility_Body *body)
{
#ifdef _REMARK_

    Matrix2d  SummaryA ;
    Matrix2d  SummaryP ;
    Matrix2d  Point ;
      double  x0, y0, z0 ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  c_x[2], c_z[2] ;    /* Точки пересечения ребер с плоскостью грани */
      double  y ;
      double  v1, v2 ;
         int  cross ;
         int  cross1, cross2 ;
         int  n ;
         int  m1[2] ;  /* Индексы вершин ребер "активного" тела, */
         int  m2[2] ;  /*  пересекающих грань "пассивного" тела  */
         int  m ;
         int  ma ;     /* Индекс "правой" вершины ребра "пассивного" тела, пересекающего грань "активного" тела */
         int  i ;

#define  F1     (b1->Facets[n1])
#define  F2     (b2->Facets[n2])
#define  V1      b1->Vertexes
#define  V2      b2->Vertexes

/*------------------------------------------------------- Подготовка */

/*---------------------------------------------- Перебор граней тела */

    for(n=0 ; n<body->Facets_cnt ; n++) {

         F1.overall.x_min=V1[F1.vertexes[0]].x_abs ;                /* Определение габаритного размера */
         F1.overall.x_max=V1[F1.vertexes[0]].x_abs ;
         F1.overall.y_min=V1[F1.vertexes[0]].y_abs ;
         F1.overall.y_max=V1[F1.vertexes[0]].y_abs ;
         F1.overall.z_min=V1[F1.vertexes[0]].z_abs ;
         F1.overall.z_max=V1[F1.vertexes[0]].z_abs ;

     for(i=1 ; i<F1.vertexes_cnt ; i++) {
       if(V1[F1.vertexes[i]].x_abs<F1.overall.x_min)  F1.overall.x_min=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].x_abs>F1.overall.x_max)  F1.overall.x_max=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].y_abs<F1.overall.y_min)  F1.overall.y_min=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].y_abs>F1.overall.y_max)  F1.overall.y_max=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].z_abs<F1.overall.z_min)  F1.overall.z_min=V1[F1.vertexes[i]].z_abs ;
       if(V1[F1.vertexes[i]].z_abs>F1.overall.z_min)  F1.overall.z_max=V1[F1.vertexes[i]].z_abs ;
                                        }
/*--------------------------------- Перебор граней "пассивного" тела */

      for(n2=0 ; n2<b2->Facets_cnt ; n2++) {
/*- - - - - - - - - - - - - - - - - - - - - -  Проверка по габаритам */
             F2.overall.x_min=V2[F2.vertexes[0]].x_abs ;            /* Определение габаритного размера */
             F2.overall.x_max=V2[F2.vertexes[0]].x_abs ;
             F2.overall.y_min=V2[F2.vertexes[0]].y_abs ;
             F2.overall.y_max=V2[F2.vertexes[0]].y_abs ;
             F2.overall.z_min=V2[F2.vertexes[0]].z_abs ;
             F2.overall.z_max=V2[F2.vertexes[0]].z_abs ;

         for(i=1 ; i<F2.vertexes_cnt ; i++) {
           if(V2[F2.vertexes[i]].x_abs<F2.overall.x_min)  F2.overall.x_min=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].x_abs>F2.overall.x_max)  F2.overall.x_max=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].y_abs<F2.overall.y_min)  F2.overall.y_min=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].y_abs>F2.overall.y_max)  F2.overall.y_max=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].z_abs<F2.overall.z_min)  F2.overall.z_min=V2[F2.vertexes[i]].z_abs ;
           if(V2[F2.vertexes[i]].z_abs>F2.overall.z_min)  F2.overall.z_max=V2[F2.vertexes[i]].z_abs ;
                                            }

           if(F1.overall.x_min>F2.overall.x_max || 
              F1.overall.x_max<F2.overall.x_min ||
              F1.overall.y_min>F2.overall.y_max ||
              F1.overall.y_max<F2.overall.y_min ||
              F1.overall.z_min>F2.overall.z_max || 
              F1.overall.z_max<F2.overall.z_min   )  continue ;
/*- - - - - - - - - - - - - - Проверка пересечения плоскости А-грани */
                   SummaryA.LoadZero(4,4) ;

              iToFlat(&V1[F1.vertexes[0]],
                      &V1[F1.vertexes[1]],
                      &V1[F1.vertexes[2]], &SummaryA) ;

                          cross=0 ;

         for(i=0 ; i<F2.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {  ma=i ;  break ;  }
                       else            cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {  ma=i ;  break ;  }
                       else          cross=-1 ;
                    }
                                            }

              if(i>=F2.vertexes_cnt)  continue ;
/*- - - - - - - - - - - - - - Проверка пересечения плоскости П-грани */
                   SummaryP.LoadZero(4,4) ;

              iToFlat(&V2[F2.vertexes[0]],
                      &V2[F2.vertexes[1]],
                      &V2[F2.vertexes[2]], &SummaryP) ;

                          cross=0 ;
                              m=0 ;

         for(i=0 ; i<F1.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=-1 ;
                    }
                                            }

              if(m==0)  continue ;

              if(m==1) {
                          m1[1]= 0 ;
                          m2[1]=F1.vertexes_cnt-1 ;
                       }
/*- - - - - - - - - - - Точки пересечения ребер с плоскостью П-грани */
         for(i=0 ; i<2 ; i++) {
                                         m=F1.vertexes[m1[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F1.vertexes[m2[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[i]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[i]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[i]=x2 ;
                         c_z[i]=z2 ;
                      }
                              }
/*- - - Проверка нахождения точек пересечения внутри контура П-грани */
/* Проверяем, что моменты цепочки векторов ребер грани относительно  */
/*  хотя бы одной из точек пересечения имеют один и тот же знак      */
                 cross1=1 ;
                 cross2=1 ;
                     v1=0 ;
                     v2=0 ;

         for(i=0 ; i<=F2.vertexes_cnt ; i++) {

           if(i<F2.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryP, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;
              y2=(z1-c_z[1])*(x2-x1)-(x1-c_x[1])*(z2-z1) ;

           if(v1*y1<0)  cross1=0 ;
           if(v2*y2<0)  cross2=0 ;

              v1=y1 ; 
              v2=y2 ; 

           if(cross1+cross2==0)  break ;         
                                            }

           if(cross1+cross2>0)  return(1) ;        
/*- - - - - - - - - - - Точки пересечения ребер с плоскостью А-грани */
                                         m=F2.vertexes[ma-1] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F2.vertexes[ma] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[0]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[0]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[0]=x2 ;
                         c_z[0]=z2 ;
                      }
/*- - - Проверка нахождения точек пересечения внутри контура П-грани */
/* Проверяем, что моменты цепочки векторов ребер грани относительно  */
/*  хотя бы одной из точек пересечения имеют один и тот же знак      */
                 cross1=1 ;
                     v1=0 ;

         for(i=0 ; i<=F1.vertexes_cnt ; i++) {

           if(i<F1.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryA, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;

           if(v1*y1<0) {  cross1=0 ;  break ;  }

              v1=y1 ; 
                                            }

           if(cross1>0)  return(1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                           }
/*---------------------------------------------- Перебор граней тела */
                                        }
/*-------------------------------------------------------------------*/

#undef  F1
#undef  F2
#undef  V1
#undef  V2

#endif

  return(0) ;

}


/********************************************************************/
/*								    */
/*             Расчет матрицы разворота плоскости x0z               */ 
/*                   в плоскость точек P0-P1-P2                     */

    int RSS_Feature_Visibility::iToFlat(RSS_Feature_Visibility_Vertex *p0,
                                        RSS_Feature_Visibility_Vertex *p1,
                                        RSS_Feature_Visibility_Vertex *p2,
                                                             Matrix2d *Transform)

{
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  v ;
      double  angle ;

/*------------------------------------------------- Сдвиг в точку P0 */

                Summary.LoadE      (4, 4) ;
                                                                    /* Точку 0 берем за базовую */ 
                  Local.Load4d_base(-p0->x_abs,
                                    -p0->y_abs,
                                    -p0->z_abs ) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      x1=p1->x_abs-p0->x_abs ;
                      x2=p2->x_abs-p0->x_abs ;
                      y1=p1->y_abs-p0->y_abs ;
                      y2=p2->y_abs-p0->y_abs ;
                      z1=p1->z_abs-p0->z_abs ;
                      z2=p2->z_abs-p0->z_abs ;

   if(fabs(z1)<fabs(z2)) { 
                            v=x1 ;  x1=x2 ; x2=v ;
                            v=y1 ;  y1=y2 ; y2=v ;
                            v=z1 ;  z1=z2 ; z2=v ;
                         }
/*----------------------------------------- Поворот вокруг Y к оси Z */

   if(z1!=0.) {
                          v=z1/sqrt(x1*x1+z1*z1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_azim(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* Перерассчитываем координаты точки 1 */
                      Point.SetCell (0, 0, x1) ;
                      Point.SetCell (1, 0, y1) ;
                      Point.SetCell (2, 0, z1) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x1=Point.GetCell (0, 0) ;
                   y1=Point.GetCell (1, 0) ;
                   z1=Point.GetCell (2, 0) ;

                      Point.LoadZero(4, 1) ;                        /* Перерассчитываем координаты точки 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- Поворот вокруг Z к оси X */

   if(y1!=0.) {
                          v=y1/sqrt(x1*x1+y1*y1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_roll(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* Перерассчитываем координаты точки 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- Поворот вокруг X к оси Y */

   if(y2!=0.) {
                          v=y2/sqrt(y2*y2+z2*z2) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_elev(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;
              }
/*-------------------------------------------------------------------*/

          Transform->Copy(&Summary) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                     Пересчет точек тел объекта                   */  

    int RSS_Feature_Visibility::RecalcPoints(void)

{
  RSS_Kernel *Calc ;
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
         int  overall_recalc ;
         int  i ;
         int  j ;

#define   B   Bodies
#define   V   Bodies[i].Vertexes

/*------------------------------------------------------- Подготовка */

           Calc=iGetCalculator() ;                                  /* Определение вычислителя */
        if(Calc==NULL)  return(-1) ;

/*---------------------------------------------- Перерсчет точек тел */

        overall_recalc=0 ;

   for(i=0 ; i<Bodies_cnt ; i++) {                                  /* CIRCLE.1 */

       if(!B[i].recalc)  continue ;                                 /* Если тело уже пересчитано... */
           B[i].recalc=0 ;
        overall_recalc=1 ;
/*- - - - - - - - - - - - - - - - - - Рассчет матрицы преобразования */
   if(i==0) {
/*
                track_s.x   =B[i].x_base ;
                track_s.y   =B[i].y_base ;
                track_s.z   =B[i].z_base ;
                track_s.azim=B[i].a_azim ;
                track_s.elev=B[i].a_elev ;
                track_s.roll=B[i].a_roll ;
*/
            }
/*- - - - - - - - - - - - - - - - - - Рассчет матрицы преобразования */
       if(B[i].Matrix_flag) {                                       /* Если задана матрица... */
                Summary.LoadArray(4, 4, (double *)B[i].Matrix) ;
                            }
       else                 {                                       /* Если заданы компоненты */
                Summary.LoadE      (4, 4) ;
                  Local.Load4d_base(B[i].x_base, B[i].y_base, B[i].z_base) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;
                            }

                  Local.Load4d_base(B[i].x_base_s, B[i].y_base_s, B[i].z_base_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
/*- - - - - - - - - - - - - - - - - - - - -  Рассчет координат точек */
     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {

           if(V[j].x_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].x, &V[j].x_calculate, NULL ) ;

           if(V[j].y_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].y, &V[j].y_calculate, NULL ) ;

           if(V[j].z_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].z, &V[j].z_calculate, NULL ) ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, V[j].x) ;
                      Point.SetCell (1, 0, V[j].y) ;
                      Point.SetCell (2, 0, V[j].z) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Summary, &Point) ;            /* Рассчитываем координаты точки */
                                                                    /*  в абсолютной СК */
           V[j].x_abs=Point.GetCell (0, 0) ;
           V[j].y_abs=Point.GetCell (1, 0) ;
           V[j].z_abs=Point.GetCell (2, 0) ;
           
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - Рассчет габаритов тела */
                        B[i].overall.x_min=V[1].x_abs ;
                        B[i].overall.x_max=V[1].x_abs ;
                        B[i].overall.y_min=V[1].y_abs ;
                        B[i].overall.y_max=V[1].y_abs ;
                        B[i].overall.z_min=V[1].z_abs ;
                        B[i].overall.z_max=V[1].z_abs ;

     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {
              if(B[i].overall.x_min>V[j].x_abs)  B[i].overall.x_min=V[j].x_abs ;
         else if(B[i].overall.x_max<V[j].x_abs)  B[i].overall.x_max=V[j].x_abs ;
              if(B[i].overall.y_min>V[j].y_abs)  B[i].overall.y_min=V[j].y_abs ;
         else if(B[i].overall.y_max<V[j].y_abs)  B[i].overall.y_max=V[j].y_abs ;
              if(B[i].overall.z_min>V[j].z_abs)  B[i].overall.z_min=V[j].z_abs ;
         else if(B[i].overall.z_max<V[j].z_abs)  B[i].overall.z_max=V[j].z_abs ;
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }                            /* CONTINUE.1 */
/*---------------------------------------------- Перерсчет габаритов */

    if(overall_recalc) {
                              overall.x_min= 1.e+99 ;
                              overall.x_max=-1.e+99 ;
                              overall.y_min= 1.e+99 ;
                              overall.y_max=-1.e+99 ;
                              overall.z_min= 1.e+99 ;
                              overall.z_max=-1.e+99 ;

      for(i=0 ; i<Bodies_cnt ; i++) {
        if(overall.x_min>B[i].overall.x_min)  overall.x_min=B[i].overall.x_min ;
        if(overall.x_max<B[i].overall.x_max)  overall.x_max=B[i].overall.x_max ;
        if(overall.y_min>B[i].overall.y_min)  overall.y_min=B[i].overall.y_min ;
        if(overall.y_max<B[i].overall.y_max)  overall.y_max=B[i].overall.y_max ;
        if(overall.z_min>B[i].overall.z_min)  overall.z_min=B[i].overall.z_min ;
        if(overall.z_max<B[i].overall.z_max)  overall.z_max=B[i].overall.z_max ;
                                    }
                       }
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

     RSS_Transit_Visibility::RSS_Transit_Visibility(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Visibility::~RSS_Transit_Visibility(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Visibility::vExecute(void)

{
   if(!stricmp(action, "SHOW"))  ((RSS_Feature_Visibility *)feature)->iShow() ;

   return(0) ;
}



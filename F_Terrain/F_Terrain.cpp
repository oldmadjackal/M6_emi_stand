/*********************************************************************/
/*								     */
/*        МОДУЛЬ УПРАВЛЕНИЯ ПРИВЯЗКОЙ К РЕЛЬЕФУ МЕСТНОСТИ            */
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

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "F_Terrain.h"

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

     static   RSS_Module_Terrain  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	Идентификационный вход                       */

 F_TERRAIN_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_TERRAIN_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**               ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ                 **/
/**	           ПРИВЯЗКОЙ К РЕЛЬЕФУ МЕСТНОСТИ	            **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            Список команд                          */

  struct RSS_Module_Terrain_instr  RSS_Module_Terrain_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                      NULL,
                     &RSS_Module_Terrain::cHelp       },
 { "slave",    "c",  "#SLAVE (S) - задание привязки объекта ", 
                     " SLAVE <Имя>\n"
                     "   задать объект как следующий рельефу местности\n",
                     &RSS_Module_Terrain::cSlave     },
 {  NULL }
                                                              } ;

/*********************************************************************/
/*								     */
/*		     Общие члены класса             		     */

    struct RSS_Module_Terrain_instr *RSS_Module_Terrain::mInstrList=NULL ;


/*********************************************************************/
/*								     */
/*		       Конструктор класса			     */

     RSS_Module_Terrain::RSS_Module_Terrain(void)

{
	   keyword="EmiStand" ;
    identification="Terrain" ;
          category="Feature" ;

        mInstrList=RSS_Module_Terrain_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса			     */

    RSS_Module_Terrain::~RSS_Module_Terrain(void)

{
}


/*********************************************************************/
/* 								     */
/*		        Стартовая разводка                           */

    void  RSS_Module_Terrain::vStart(void)

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

    RSS_Feature *RSS_Module_Terrain::vCreateFeature(RSS_Object *object, RSS_Feature *feature_ext)

{
  RSS_Feature *feature ;


    if(feature_ext!=NULL)  feature= feature_ext ;
    else                   feature=new RSS_Feature_Terrain ;

         feature->Object=object ;

  return(feature) ;
}


/*********************************************************************/
/*								     */
/*		        Выполнить команду       		     */

  int  RSS_Module_Terrain::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "TERRAIN"
#define  _SECTION_SHRT_NAME   "TER"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Terrain features:") ;
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
       if(status)  SEND_ERROR("Секция TERRAIN: Неизвестная команда") ;
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

    void  RSS_Module_Terrain::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE TERRAIN\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef   MARKS_LIST
#undef   MARKS_LIST_CNT

}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Terrain::vReadSave(std::string *data)

{
                    char *buff ;
                     int  buff_size ;
             std::string  decl ;
                    char *work ;
                    char  text[1024] ;
                    char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE TERRAIN\n", 
                      strlen("#BEGIN MODULE TERRAIN\n")))  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- Построчная разборка */

                work=buff+strlen("#BEGIN MODULE TERRAIN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------------- ??????? */

          if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- Неизвестный спецификатор */

     else                                                {

                sprintf(text, "Module TERRAIN: "
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

  int  RSS_Module_Terrain::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Terrain_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SLAVE                   */
/*								    */
/*        SLAVE <Имя>                                               */

  int  RSS_Module_Terrain::cSlave(char *cmd)

{
#define   _PARS_MAX   4

               char *pars[_PARS_MAX] ;
               char *name ;
         RSS_Object *object ;
               char  text[1024] ;
               char *end ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;

/*------------------------------------------- Поиск объекта по имени */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: SLAVE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                         object=OBJECTS[i] ;
                                                 break ;
                                              }

    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*--------------------------------------------- Назначение категории */

   for(i=0 ; i<object->Features_cnt ; i++)
     if(!stricmp(object->Features[i]->Type, "Terrain")) {
          ((RSS_Feature_Terrain *)(object->Features[i]))->slave=1 ;
                                                    }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**           ОПИСАНИЕ КЛАССА ПРИВЯЗКИ К РЕЛЬЕФУ МЕСТНОСТИ         **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Статические переменные			    */


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Feature_Terrain::RSS_Feature_Terrain(void)

{
   strcpy(Type, "Terrain") ;

       Bodies    =NULL ;
       Bodies_cnt=  0 ;

            slave=  0 ;

    terrain_last =NULL ;
    terrain_body = -1 ;
    terrain_facet= -1 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Feature_Terrain::~RSS_Feature_Terrain(void)

{
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Feature_Terrain::vReadSave(       char *title,  
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

       if(stricmp(title, "FEATURE TERRAIN"))  return ;

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

               Bodies=(RSS_Feature_Terrain_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
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
                sprintf(text, "Section FEATURE TERRAIN: "
                              "Неверный формат спецификатора VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  Расширение списка точек */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Terrain_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: Недостаточно памяти") ;
                                   break ;
                               }                    

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - Выделение описателей X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* Выделяем память под описание */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: Недостаточно памяти") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
                                "Неверный формат спецификатора VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
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
                  sprintf(text, "Section FEATURE TERRAIN: Ошибка вычислителя\n\n"
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
                sprintf(text, "Section FEATURE TERRAIN: "
                              "Неверный формат спецификатора FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - Добавление новой грани */
              FACETS=(RSS_Feature_Terrain_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: Недостаточно памяти") ;
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
                  SEND_ERROR("Section FEATURE TERRAIN: Слишком много вершин у грани") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- Неизвестный спецификатор */

     else                                                {

                sprintf(text, "Section FEATURE TERRAIN: "
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

    void RSS_Feature_Terrain::vGetInfo(std::string *text)

{
   if(slave)  *text="Terrain: Slave\r\n" ;
   else       *text="Terrain: Free\r\n" ;
}


/*********************************************************************/
/*								     */
/*                          Удалить тело                             */

    void RSS_Feature_Terrain::vBodyDelete(char *body)
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

    void RSS_Feature_Terrain::vBodyBasePoint(  char *body,                   
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

    void RSS_Feature_Terrain::vBodyAngles(  char *body,                   
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

    void RSS_Feature_Terrain::vBodyMatrix(  char *body,
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

    void RSS_Feature_Terrain::vBodyShifts(   char *body,                   
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

    void RSS_Feature_Terrain::vBodyPars(char *body, struct RSS_Parameter *pars)
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

  RSS_Kernel *RSS_Feature_Terrain::iGetCalculator(void)

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

    int RSS_Feature_Terrain::vResetCheck(void *data)

{
            terrain_last =NULL ;
            terrain_body = -1 ;
            terrain_facet= -1 ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*       Подготовка к проверке непротиворечивости свойства           */  

    int RSS_Feature_Terrain::vPreCheck(void *data)

{
         this->RecalcPoints() ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Проверка непротиворечивости свойства              */  

    int RSS_Feature_Terrain::vCheck(void *data, RSS_Objects_List *checked)

{
 RSS_Object *object ;
        int  n ;            /* Индекс объекта в списке объектов*/
        int  m ;            /* Индекс свойства "Пересечение" в списке свойств */
        int  i ;
        int  j ;

#define  OBJECTS_CNT                     RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                         RSS_Kernel::kernel_objects
#define  TER         ((RSS_Feature_Terrain *)object->Features[m])

/*------------------------------------------------ Входной контроль  */

      if(this->slave==0)  return(0) ;                               /* Если объект не следует по рельефу... */

/*----------------- Проверка предыдущего связанного элемента рельефа */

  if(terrain_last!=NULL) do {

        if(!iOverallTest(terrain_last))  {
                                            terrain_last=NULL ;
                                               break ;
                                         } 

        if(iFacetTest(terrain_last, 
                      terrain_body, terrain_facet))   break ;

                 terrain_body=-1 ;

      for(i=0 ; i<terrain_last->Bodies_cnt ; i++) {                 /* Анализ элементов участка  */

        for(j=0 ; j<terrain_last->Bodies[i].Facets_cnt ; j++) 
          if(iFacetTest(terrain_last, i, j)) {
                                                terrain_body =i ;
                                                terrain_facet=j ;
                                                    break ;
                                             }
           if(terrain_body>=0)  break ;

                                                  }

           if(terrain_body==-1)  terrain_last=NULL ;

                            } while(0) ;

/*------------------------------------------------- Обсчет поражения */

  if(terrain_last==NULL) {

      for(n=0 ; n<OBJECTS_CNT ; n++) {                              /* CIRCLE.1 - Перебор объектов */

           object=OBJECTS[n] ;

        for(m=0 ; m<object->Features_cnt ; m++)                    /* Проверяем, связано ли с данным объектом         */
          if(!strcmp(object->Features[m]->Type,                    /*  свойство "Пересечение", если нет - обходим его */
                                    this->Type ))  break ;  

            if(m==object->Features_cnt)  continue ;                /* Если объект-цель не обладает своиством Terrain... */

            if(TER->slave        )  continue ;                     /* Только для элементов рельефа */
            if(TER->Bodies_cnt==0)  continue ;                     /* Если свойство Terrain - пустое */
    
            if(!iOverallTest(TER))  continue ;                     /* Габаритная проверка... */

                 terrain_body=-1 ;

          for(i=0 ; i<TER->Bodies_cnt ; i++) {                     /* Анализ элементов участка  */

            for(j=0 ; j<TER->Bodies[i].Facets_cnt ; j++) 
              if(iFacetTest(TER, i, j)) {
                                             terrain_last =TER ;
                                             terrain_body = i ;
                                             terrain_facet= j ;
                                                 break ;
                                        }

              if(terrain_last!=NULL)  break ;

                                             }

                                     }                              /* CONTINUE.1 */

                         }
/*------------------------------ Привязка объекта к элементу рельефа */

   if(terrain_last!=NULL) {


                          }
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef   TER

   return(0) ;
}


/********************************************************************/
/*								    */
/*                    Проверка пересечения габаритов                */ 

    int RSS_Feature_Terrain::iOverallTest(RSS_Feature_Terrain *terrain)
{

   if(this->Object->x_base>=terrain->overall.x_min &&
      this->Object->x_base<=terrain->overall.x_max &&
      this->Object->z_base>=terrain->overall.z_min &&
      this->Object->z_base<=terrain->overall.z_max   )  return(1) ;

  return(0) ;
}     


/********************************************************************/
/*								    */
/*                Проверка пересечения граней тела цели             */ 

    int RSS_Feature_Terrain::iFacetTest(RSS_Feature_Terrain *terrain, int  n_body, int  n_facet)
{
   RSS_Feature_Terrain_Body *body ;
  RSS_Feature_Terrain_Facet *facet ;
                     double  x0, z0 ;
                     double  x1, z1 ;
                     double  x2, z2 ;
                     double  x_b, z_b ;
                     double  y ;
                     double  v ;
                        int  cross ;
                        int  n ;
                        int  i ;

/*------------------------------------------------------- Подготовка */

         body=&terrain->Bodies[n_body ] ;
        facet=&   body->Facets[n_facet] ;

          x_b=this->Object->x_base ;
          z_b=this->Object->z_base ;

/*----------------- Проверка нахождения объекта внутри контура грани */
/* Проверяем, что моменты цепочки векторов ребер грани относительно  */
/*  базовой точки объекта имеют один и тот же знак                   */
                 cross =1 ;
                     v =0 ;
                     x2=0 ;
                     z2=0 ;

   for(i=0 ; i<=facet->vertexes_cnt ; i++) {

     if(i<facet->vertexes_cnt) {

                 x1=x2 ;
                 z1=z2 ;
                 x2=body->Vertexes[facet->vertexes[i]].x_abs ;
                 z2=body->Vertexes[facet->vertexes[i]].z_abs ;
                               }
     else                      {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                               }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y=(z1-z_b)*(x2-x1)-(x1-x_b)*(z2-z1) ;

           if(v*y<0)  cross=0 ;

                v=y ; 

           if(cross==0)  break ;         
                                           }

           if(cross>0)  return(1) ;

/*-------------------------------------------------------------------*/

  return(0) ;

}

#ifdef _REMARK

/********************************************************************/
/*								    */
/*             Расчет матрицы разворота плоскости x0z               */ 
/*                   в плоскость точек P0-P1-P2                     */

    int RSS_Feature_Terrain::iToFlat(RSS_Feature_Terrain_Vertex *p0,
                                     RSS_Feature_Terrain_Vertex *p1,
                                     RSS_Feature_Terrain_Vertex *p2,
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
/*--------------------------------------------------------- Проверка */
/*
                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p0->x_abs) ;
                 Point.SetCell (1, 0, p0->y_abs) ;
                 Point.SetCell (2, 0, p0->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p1->x_abs) ;
                 Point.SetCell (1, 0, p1->y_abs) ;
                 Point.SetCell (2, 0, p1->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p2->x_abs) ;
                 Point.SetCell (1, 0, p2->y_abs) ;
                 Point.SetCell (2, 0, p2->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;
*/
/*-------------------------------------------------------------------*/

          Transform->Copy(&Summary) ;

   return(0) ;
}

#endif


/********************************************************************/
/*								    */
/*                     Пересчет точек тел объекта                   */  

    int RSS_Feature_Terrain::RecalcPoints(void)

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


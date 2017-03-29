
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>

#include <windows.h>

#include "..\RSS_feature\RSS_feature.h"
#include "..\RSS_object\RSS_object.h"
#include "RSS_Kernel.h"

#pragma warning(disable : 4996)


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
/*		       Статические переменные			    */

 RSS_KERNEL_API    RSS_Kernel  *RSS_Kernel::kernel               =NULL ;
 RSS_KERNEL_API          HWND   RSS_Kernel::kernel_wnd           =NULL ;
 RSS_KERNEL_API          HWND   RSS_Kernel::active_wnd           =NULL ;
 RSS_KERNEL_API     HINSTANCE   RSS_Kernel::kernel_inst          =NULL ;

 RSS_KERNEL_API           int   RSS_Kernel::stop                 =  0 ; 

 RSS_KERNEL_API    RSS_Object **RSS_Kernel::kernel_objects       =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::kernel_objects_cnt   =  0 ; 

 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::feature_modules      =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::feature_modules_cnt  =  0 ; 

 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::calculate_modules    =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::calculate_modules_cnt=  0 ; 

 RSS_KERNEL_API    RSS_Result **RSS_Kernel::results              =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::results_cnt          =  0 ;

 RSS_KERNEL_API    RSS_Kernel  *RSS_Kernel::priority_entry       =NULL ;
 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::events_entry         =NULL ;
 RSS_KERNEL_API           int   RSS_Kernel::events_entry_cnt     =  0 ;

 RSS_KERNEL_API  RSS_CB_Entry   RSS_Kernel::callbacks[_CALLB_MAX] ; 

 RSS_KERNEL_API   RSS_Display   RSS_Kernel::display ;

 RSS_KERNEL_API           int   RSS_Kernel::srand_fixed ;

 RSS_KERNEL_API           int   RSS_Kernel::debug_flag           =  0 ;
 RSS_KERNEL_API          char   RSS_Kernel::debug_list[1024] ;

/********************************************************************/
/*								    */
/*		    Назначение модуля - ядра			    */

  void  RSS_Kernel::SetKernel(HWND wnd, HINSTANCE inst)

{
      kernel     =this ;
      kernel_wnd =wnd ;
      kernel_inst=inst ;
}


/********************************************************************/
/*								    */
/*		     Задание функции обратной связи		    */

  RSS_Kernel_CallBack  RSS_Kernel::SetCallB(char *name, 
                                             RSS_Kernel_CallBack  proc)

{
   int  i ;


    for(i=0 ; i<_CALLB_MAX ; i++)
      if(!strcmp(callbacks[i].name, name)   ||
		 callbacks[i].name[0]==  0  ||
		 callbacks[i].proc   ==NULL   )  {

			 strcpy(callbacks[i].name, name) ;
				callbacks[i].proc=proc ;
				    break ;
					         }
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		     Запрос функции обратной связи		    */

  RSS_Kernel_CallBack  RSS_Kernel::GetCallB(char *name)

{
   int  i ;


    for(i=0 ; i<_CALLB_MAX ; i++)
      if(!strcmp(name, callbacks[i].name))  return(callbacks[i].proc) ;

   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		    Регистрация ресурса				    */

  void *RSS_Kernel::Resource(const char *name, const char *type)

{
    HRSRC  hRes ;    
  HGLOBAL  hData ;
     void *ptr ;
      int  i ;

/*----------------------------------------- Проверка наличия ресурса */

    for(i=0 ; i<resources_cnt ; i++)
     if(!strcmp(name, resources[i].name))  return(resources[i].ptr) ;

/*------------------------------------------------- Загрузка ресурса */

	hRes=FindResource(DLL_module, name, type) ;
     if(hRes==NULL)  return(NULL) ;

        hData=LoadResource(DLL_module, hRes) ;
     if(hData==NULL)  return(NULL) ;

        ptr=LockResource(hData) ;
     if(ptr==NULL)  return(NULL) ;

/*---------------------------------------------- Регистрация ресурса */

     resources=(RSS_Resource *)
                  realloc(resources, sizeof(*resources)*
					 (resources_cnt+1)) ;
  if(resources==NULL) {
			resources_cnt=0 ;
			     return(NULL) ; ;
		      }

     strcpy(resources[resources_cnt].name, name) ;
	    resources[resources_cnt].ptr=ptr ;
                      resources_cnt++ ;

/*-------------------------------------------------------------------*/

    return(ptr) ;
}


/********************************************************************/
/*								    */
/*		      Загрузка используемых модулей		    */
/*								    */
/*	path - путь к разделу используемых хранения модулей	    */

typedef       char *(*KERNEL_API)(void);
typedef RSS_Kernel *(*MODULE_PTR)(void);

   int  RSS_Kernel::Load(const char *path)

{
            char  dll_mask[512] ;   /* Путь к разделу DLL */
             int  dll_group ;	    /* Дескриптор поисковой группы DLL */
     _finddata_t  dll_file ;	    /* Описание найденного файла */
            char  dll_path[512] ;   /* Полный путь к файлу DLL */
	 HMODULE  module ;	    /* Дескриптор DLL */
      KERNEL_API  identify ;	    /* Адрес процедуры идентфикации DLL */
      MODULE_PTR  link ;	    /* Адрес процедуры связи с модулем */
	    char  message[512] ;    
             int  status ;
  

/*------------------------------------------------------- Подготовка */

                      strcpy(dll_mask, path) ;
                      strcat(dll_mask, "*.dll") ;

/*--------------------------------------------------- Стыковка с DLL */

        dll_group=_findfirst(dll_mask, &dll_file);
     if(dll_group<0) {
	      sprintf(message, "No DLL on path: %s", dll_mask) ;
	    iErrorMsg(message) ;
	  	        return(-1) ;
		     }

  while(dll_group) {						    /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Загрузка DLL */
	          sprintf(dll_path, "%s%s", path, dll_file.name) ;
       module=LoadLibrary(dll_path);
    if(module==0) {						    /* IF.1 - Если ошибка загрузки DLL... */
		       sprintf(message, "DLL load failure: %s", dll_path) ;
		     iErrorMsg(message) ;
		  }						    /* ELSE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Идентификация DLL */
    else	  {						    /* ELSE.1 - Если DLL загружена... */

	        identify=(KERNEL_API)GetProcAddress(module,         /* Адресуем процедуру идентификации DLL */
                                                    "Identify");

      if(       identify==NULL     ||				    /* IF.2 - Если идентификация не прошла... */
	 strcmp(identify(), keyword) ) {
					  FreeLibrary(module) ;
				       }			    /* ELSE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Регистрация DLL */
      else			       {			    /* ELSE.2 - Если это нужная DLL... */

	   link=(MODULE_PTR)GetProcAddress(module, "GetEntry");     /* Адресуем процедуру связи с модулем */
	if(link==NULL) {					    /* Если неудачно... */
		           sprintf(message, "DLL registration error %d: %s",
					   GetLastError(), dll_path) ;
		         iErrorMsg(message) ;		
		       }
	else	       {					    /* Регистрируем модуль... */
	      modules=(RSS_Module_Entry *)
                         realloc(modules, (modules_cnt+1)*sizeof(*modules)) ;
		modules[modules_cnt].entry =link() ; 
		modules[modules_cnt].module=module ; 
		
		modules[modules_cnt].entry->DLL_module=module ;	    /* На загруженном модуле          */
								    /*   фиксируем дескриптор его DLL */
		        modules_cnt++ ;
		       }
				       }			    /* END.2 */
		  }						    /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - -  Поиск следующей DLL */
		status=_findnext(dll_group, &dll_file) ;
	     if(status!=0)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	           }						    /* CONTINUE.1 */

	   if(dll_group)  _findclose(dll_group) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		    Освобождение используемых модулей		    */

   int  RSS_Kernel::Free(void)

{
  int  i ;

    
    if(modules_cnt) {
                	for(i=0 ; i<modules_cnt ; i++)
               	              FreeLibrary(modules[i].module) ;

	                             free(modules) ;
	    	                          modules    =NULL ;
		                          modules_cnt=  0 ;
                    }

  return(0) ;
}


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Kernel::RSS_Kernel(void)

{
          keyword    =NULL ;
   identification    =NULL ;

	  modules    =NULL ;
	  modules_cnt=  0 ;
	resources    =NULL ;
	resources_cnt=  0 ;

	 priority    =  0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Kernel::~RSS_Kernel(void)

{
}


/********************************************************************/
/*								    */
/*	                 Обработка событий                          */	

  int  RSS_Kernel::vKernelEvents(void)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*		Формирование сообщения об ошибке		    */	

   void  RSS_Kernel::iErrorMsg(const char *message)

{  
     MessageBox(GetActiveWindow(), message, "RSS Kernel", MB_ICONERROR) ;
       iLogFile(message) ;
}


/********************************************************************/
/*								    */
/*		Запись сообщения в лог-файл			    */	

   void  RSS_Kernel::iLogFile(const char *message)

{
         FILE *file ;         

       file=fopen("kernel.log", "at") ;
    if(file==NULL)  return ;

           fwrite(this->identification, 1, strlen(this->identification), file) ;
           fwrite(     "\t",            1, strlen(     "\t"           ), file) ;
           fwrite(      message,        1, strlen(      message       ), file) ;
           fwrite(     "\n",            1, strlen(     "\n"           ), file) ;
           fclose(file) ;
}


/********************************************************************/
/*								    */
/*		        Стартовая разводка                          */

    void  RSS_Kernel::vStart(void)

{
}


/********************************************************************/
/*								    */
/*		        Инициализация связей                        */

    void  RSS_Kernel::vInit(void)

{
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Kernel::vReadSave(std::string *text)

{
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Kernel::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		        Создать объект                  	    */

    RSS_Object *RSS_Kernel::vCreateObject(void)

{
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		        Создать свойство                	    */

    RSS_Feature *RSS_Kernel::vCreateFeature(RSS_Object *object)

{
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

     int  RSS_Kernel::vExecuteCmd(const char *command)

{
   return(1) ;
}

     int  RSS_Kernel::vExecuteCmd(const char *command, RSS_IFace *iface)

{
   return(vExecuteCmd(command)) ;
}


/********************************************************************/
/*								    */
/*		        Вычислить выражение     		    */

    int  RSS_Kernel::vCalculate(            char  *expression_type,
                                            char  *expression, 
                            struct RSS_Parameter  *const_list,
                            struct RSS_Parameter  *var_list,
                                          double  *result,
                                            void **context, 
                                            char  *error)

{

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */
/*		        Задать параметр         		    */

     int  RSS_Kernel::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Unknown module", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


    void  RSS_Kernel::vSetParameter(char *name, char *value)

{
}


/********************************************************************/
/*								    */
/*                   Выполнить целевой функционал                   */

    void  RSS_Kernel::vProcess(void)

{
}


/********************************************************************/
/*								    */
/*                    Отобразить связанные данные                   */

    void  RSS_Kernel::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*             Выполнить действие в контексте потока                */

    void  RSS_Kernel::vChangeContext(void)

{
}


/********************************************************************/
/*								    */
/*           Считать данные контекста модуля из строки              */

    int  RSS_Kernel::vReadData(RSS_Context ***context,
                               std::string   *text    )
{
   return(0) ;
}


/********************************************************************/
/*								    */
/*            Записать данные контекста модуля в строку             */

   void  RSS_Kernel::vWriteData(RSS_Context *context,
                                std::string *text    )
{
   *text="" ;
}


/********************************************************************/
/*								    */
/*            Освободить ресурсы данных контекстове модуля          */

    void  RSS_Kernel::vReleaseData(RSS_Context *context)

{
     if(context==NULL)  return ;

        free(context->data) ;
        free(context) ;
}


/********************************************************************/
/*								    */
/*                Ввод контекста в список контекстов                */

     RSS_Context *RSS_Kernel::vAddData(RSS_Context ***contexts)

{
  int  j ;

#define  C   (*contexts)

/*---------------------------------- Инициализация списка контекстов */

       if(C==NULL)                                                  /* Если список контекстов пуст -  */
          C=(RSS_Context **)calloc(1, sizeof(*C)) ;                 /*   - инициализируем его         */
       if(C==NULL)  return(NULL) ;                                  /* Если не удалось разместить... */
                         
/*------------------------------------------- Поиск контекста модуля */

      for(j=0 ; C[j]!=NULL ; j++)                                   /* Ищем контекст этого модуля */
        if(!strcmp(C[j]->name, this->identification))  break ;

       if(C[j]!=NULL)  return(C[j]) ;                               /* Если контекст найден -> все нормально */

/*---------------------------------------- Создание контекста модуля */

          C=(RSS_Context **)realloc(C, (j+2)*sizeof(*C)) ;          /* Доразмещаем список контекстов */
       if(C==NULL)  return(NULL) ;                                  /* Если не удалось разместить... */

          C[j+1]=  NULL ;                                           /* Терминируем список контекстов */
          C[j  ]=(RSS_Context *)calloc(1, sizeof(**C)) ;            /* Размещаем контекст */
       if(C[j  ]==NULL)  return(NULL) ;                             /* Если не удалось разместить... */

/*-------------------------------- Пропись идентификаторов контекста */

         strcpy(C[j]->name, this->identification) ;                 /* Пропись идентификатора */
                C[j]->module=this ;                                 /* Пропись мастер-модуля */

/*-------------------------------------------------------------------*/

   return(C[j]) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**                        УПРАВЛЕНИЕ ПАМЯТЬЮ                      **/
/**								   **/
/********************************************************************/
/********************************************************************/


 RSS_KERNEL_API          int   RSS_Kernel::memchk_regime  =  0 ;
 RSS_KERNEL_API   RSS_Memory  *RSS_Kernel::memchk_list    =NULL ;
 RSS_KERNEL_API          int   RSS_Kernel::memchk_list_max=  0 ; 
 RSS_KERNEL_API          int   RSS_Kernel::memchk_list_idx=  0 ; 
 RSS_KERNEL_API          int   RSS_Kernel::memchk_rep_num =  0 ; 


/********************************************************************/
/*								    */
/*                         Аналог функции CALLOC                    */

   void *RSS_Kernel::gMemCalloc(size_t  n, size_t  size, 
                                  char *entry, int  idx1, int  idx2 )
{
  void *ptr ;

          ptr=calloc(n, size) ;
         iMemAddList(ptr, size*n, entry, idx1, idx2) ;

   return(ptr) ;
}


/********************************************************************/
/*								    */
/*                         Аналог функции REALLOC                   */

   void *RSS_Kernel::gMemRealloc(void *ptr, size_t  size, 
                                 char *entry, int  idx1, int  idx2 )
{
        iMemDelList(ptr) ;
        ptr=realloc(ptr, size) ;
        iMemAddList(ptr, size, entry, idx1, idx2) ;

   return(ptr) ;
}


/********************************************************************/
/*								    */
/*                         Аналог функции FREE                      */

   void  RSS_Kernel::gMemFree(void *ptr)
{
               free(ptr) ;
        iMemDelList(ptr) ;
}


/********************************************************************/
/*								    */
/*                         Аналог функции HEAPCHK                   */

   int  RSS_Kernel::gMemCheck(void)
{
  int  status ;


       status=_heapchk() ;
    if(status==_HEAPBADBEGIN ||
       status==_HEAPBADNODE  ||
       status==_HEAPBADPTR     ) {
                                    return(-1) ;
                                 } 

    if(status==_HEAPOK &&
        errno== ENOSYS   ) {
                                    return( 1) ;
                           } 

                                    return( 0) ;
}


/********************************************************************/
/*								    */
/*               Вкл./выкл. мониторинга динамической памяти         */

   void  RSS_Kernel::gMemOnOff(int  regime) 
{
        memchk_regime=regime ;
}


/*********************************************************************/
/*                                                                   */
/*                Выдача распределения памяти в файл                 */

   void  RSS_Kernel::gMemList(char *path) 
{
       FILE *file ;
       char  file_name[1024] ;
       char *star ;
  struct tm *hhmmss ;           
       char  text[1024] ;
        int  i ;

/*------------------------------------------------- Входной контроль */

     if(!memchk_regime)  return ;

     if(memchk_list==NULL)  return ;

/*-------------------------------- Формирование имени файла по маске */

              strcpy(file_name, path) ;
         star=strchr(file_name, '*') ;

      if(star!=NULL) {
                                              memchk_rep_num++ ;
                        sprintf(text, "%06d", memchk_rep_num) ;

                        memmove(star+5, star, strlen(star)+1) ;
                         memcpy(star, text, 6) ;
                     }
/*-------------------------------------------- Открытие файла отчета */

        file=fopen(file_name, "wt") ;
     if(file==NULL) {
                       iErrorMsg("Memory list file open error") ;
                              return ;
                    }
/*------------------------------------------- Список активных входов */

        sprintf(text, "\n*** ACTIVE ENTRIES ***\n\n") ;
         fwrite(text, 1, strlen(text), file) ;

        for(i=0 ; i<memchk_list_max ; i++) {

          if(memchk_list[i].used!=1)  continue ;

               hhmmss=localtime(&memchk_list[i].time) ;

           sprintf(text, "%02d:%02d:%02d %02d.%02d.%02d %p Size %-7d Idx=%03d/%03d Entry=%s\n",
                          hhmmss->tm_hour, hhmmss->tm_min, hhmmss->tm_sec,
                          hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year-100,
                          memchk_list[i].ptr,
                          memchk_list[i].size,
                          memchk_list[i].idx1,
                          memchk_list[i].idx2,
                          memchk_list[i].entry ) ;
            fwrite(text, 1, strlen(text), file) ;
                                           }

/*-------------------------------------------- Закрытие файла отчета */

        fclose(file) ;

/*-------------------------------------------------------------------*/   
}


/*********************************************************************/
/*                                                                   */
/*                  Зарегистрировать вход                            */
  
  void  RSS_Kernel::iMemAddList(void *ptr, int size, 
                                char *entry, int idx1, int idx2) 

{
  int  i ;

/*------------------------------------------------- Входной контроль */

     if(!memchk_regime)  return ;

     if(ptr==NULL)  return ;

/*------------------------------------------- Поиск свободного входа */

    for(i=0 ; i<memchk_list_max ; i++)
      if(memchk_list[i].used==0)  break ;

/*----------------------------------------- Дополнение списка входов */

      if(i==memchk_list_max) {

           memchk_list_max+=  100 ;
           memchk_list     =(RSS_Memory *)
                             realloc(memchk_list,
                                     memchk_list_max*sizeof(memchk_list[0])) ;
        if(memchk_list==NULL) {
                                     return ;
                              }  

             memset(&memchk_list[i], 0,
                    (memchk_list_max-i)*sizeof(memchk_list[0])) ;
                             }
/*------------------------------------------------ Регистрация входа */

        strncpy(memchk_list[i].entry, entry, sizeof(memchk_list[i].entry)-1) ;
                memchk_list[i].idx1=idx1 ;
                memchk_list[i].idx2=idx2 ;
                memchk_list[i].ptr =ptr ;
                memchk_list[i].size=size ;
                memchk_list[i].used= 1 ;
                memchk_list[i].time=time(NULL)  ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                       Освободить вход                             */

  void  RSS_Kernel::iMemDelList(void *ptr)
{
   char  text[1024] ;
    int  i ;

/*------------------------------------------------- Входной контроль */

     if(!memchk_regime)  return ;

     if(ptr==NULL)  return ;

     if(memchk_list    ==NULL ||
        memchk_list_max==  0    )  return ;

/*------------------------------------------------------ Поиск входа */

    for(i=0 ; i<memchk_list_max ; i++)
      if(memchk_list[i].used== 1 &&
         memchk_list[i].ptr ==ptr  )  break ;

/*---------------------------------------------- Если вход не найден */

      if(i==memchk_list_max) {
/*- - - - - - - - - - - - - - - - - Поиск среди освобожденных входов */
        for(i=0 ; i<memchk_list_max ; i++)
          if(memchk_list[i].used==-1 &&
             memchk_list[i].ptr ==ptr  )  break ;


          if(i==memchk_list_max) {
                 sprintf(text, "Memory %p unregistered", ptr) ;                                
               iErrorMsg(text) ;
                                     return ;
                                 }

                 sprintf(text, "Memory %p already freed: <%s>.%d.%d",
                                             ptr, memchk_list[i].entry,
                                                  memchk_list[i].idx1,
                                                  memchk_list[i].idx2 ) ;
               iErrorMsg(text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                             }
/*----------------------------------------------- Освобождение входа */

    if(memchk_regime==_MEMCHECK_HISTORY)  memchk_list[i].used=-1 ;
    else                                  memchk_list[i].used= 0 ;    

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**                ИНТЕРФЕЙС МЕЖМОДУЛЬНОЙ СВЯЗИ                    **/
/**								   **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_IFace::RSS_IFace(void)

{
   this->std_iface=(char *)calloc(1, 64000) ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_IFace::~RSS_IFace(void)

{
   if(this->std_iface!=NULL)  free(this->std_iface) ;
}


/********************************************************************/
/*								    */
/*                          Сбросить данные                         */

   void  RSS_IFace::vClear(void)
{
   if(this->std_iface!=NULL)  this->std_iface[0]=0 ;
}


/********************************************************************/
/*								    */
/*                          Сформировать сигнал                     */

   void  RSS_IFace::vSignal(char *signal, void *data)
{
   if(this->std_iface!=NULL)  
         sprintf(this->std_iface, "%s:%s", signal, data) ;
}


/********************************************************************/
/*								    */
/*                          Tранзит сигнала                         */

   void  RSS_IFace::vPass(RSS_IFace *iface)
{
   if( this->std_iface!=NULL &&
      iface->std_iface!=NULL   )  
         strcpy(this->std_iface, iface->std_iface) ;
}


/********************************************************************/
/*								    */
/*                        Разбор сигнала                            */

    int  RSS_IFace::vDecode(char *signal, void *data)
{
   char *end ;


    if(signal!=NULL)  *signal=0 ;

    if(this->std_iface!=NULL) {

          end=strchr(this->std_iface, ':') ;
       if(end!=NULL) {
                               *end=0 ;
         if(signal!=NULL)  strcpy(signal, this->std_iface) ;
         if(data  !=NULL)  strcpy((char *)data,   end+1) ;
                               *end=':'  ;
                     }
       else          {

         if(signal!=NULL)  strcpy(signal, this->std_iface) ;
                     }
                               }

  if( signal==NULL ||  
     *signal==  0    )  return(0) ;
                        return(1) ;
}


/********************************************************************/
/*								    */
/*                         Проверка сигнала                         */

    int  RSS_IFace::vCheck(char *signal)
{

    if(this->std_iface==NULL)  return(0) ;

    if(signal==NULL) {

            if(this->std_iface[0]!=0)  return(1) ;

                      }
     else             {

            if(strlen(this->std_iface)<strlen(signal))  return(0) ;
              
            if(memicmp(this->std_iface, signal, 
                                       strlen(signal)))  return(0) ;

            if(this->std_iface[strlen(signal)]!=':' &&
               this->std_iface[strlen(signal)]!= 0    )  return(0) ;

                                                         return(1) ;
                      }

   return(0) ;
}



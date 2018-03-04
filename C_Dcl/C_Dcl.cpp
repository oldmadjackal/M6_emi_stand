/********************************************************************/
/*								    */
/*		МОДУЛЬ СТАНДАРТНОГО ВЫЧИСЛИТЕЛЯ        	            */
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

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "..\DCL_kernel\dcl.h"

#include "C_Dcl.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,        \
                                         (LPARAM) text) ;

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

     static   RSS_Calc_Dcl  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 C_DCL_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 C_DCL_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/*								    */
/*		    	Буфер текста                                */

   static  char *RSS_Calc_Dcl_text    =NULL ;
   static  char *RSS_Calc_Dcl_text_cur=NULL ;

/********************************************************************/
/********************************************************************/
/**							           **/
/**                  ОПИСАНИЕ КЛАССА МОДУЛЯ                        **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Calc_Dcl::RSS_Calc_Dcl(void)

{
	   keyword="EmiStand" ;
    identification="DCL_processor" ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

     RSS_Calc_Dcl::~RSS_Calc_Dcl(void)

{
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Calc_Dcl::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Процессор сценариев", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Стартовая разводка                          */

    void  RSS_Calc_Dcl::vStart(void)

{
   calculate_modules=(RSS_Kernel **)
                      realloc(calculate_modules, 
                              (calculate_modules_cnt+1)*sizeof(calculate_modules[0])) ;

      calculate_modules[calculate_modules_cnt]=&ProgramModule ;
                        calculate_modules_cnt++ ;
}


/********************************************************************/
/*								    */
/*		        Вычислить выражение     		    */

     extern double  dcl_errno ;            /* Системный указатель ошибок -> DCL_SLIB.CPP */
     extern   char  dcl_err_details[512] ; /* Системный указатель расширенного описания ошибок -> DCL_SLIB.CPP */

//             int ERPC_dcl_debug(void) ;


  int  RSS_Calc_Dcl::vCalculate(            char  *program_type, 
                                            char  *program, 
                            struct RSS_Parameter  *const_list,
                            struct RSS_Parameter  *var_list,
                                          double  *result,
                                            void **context_ptr, 
                                            char  *error       )

{
              Lang_DCL  DCL ;
  RSS_Calc_Dcl_context *context ;
                   int  i ;

#define  _SYSVARS_MAX    100

     Dcl_decl *vars[10] ;
          int  vars_cnt ;
         char *libs[10] ;
     Dcl_decl  sys_vars[_SYSVARS_MAX] ;
         char  crn_path[512] ;
          int  j ;

 extern Dcl_decl  dcl_debug_lib[] ;  
 extern Dcl_decl  dcl_std_lib[] ;
 extern Dcl_decl  dcl_file_lib[] ;   

#define        _BUFF_SIZE   64000

/*---------------------------------------- Идентификация вычислителя */

   if(stricmp(program_type, "DCL"  ) &&
      stricmp(program_type, "CLEAR")   )  return(1) ;

   if(context_ptr==NULL)  return(0) ;



/*------------------------------------------------ Очистка контекста */

   if(!stricmp(program_type, "CLEAR")) {

          context=(RSS_Calc_Dcl_context *)*context_ptr ;
       if(context==NULL)  return(0) ;

                              free(context) ;

                        *context_ptr=NULL ;

                           return(0) ;
                                       }
/*---------------------------------- Подключение системных библиотек */

       memset(vars, 0, sizeof(vars)) ;

                vars_cnt = 0 ;
	   vars[vars_cnt]=sys_vars ;                                       /* Список переменных и функций */
                vars_cnt++ ;
	   vars[vars_cnt]=dcl_debug_lib ;
                vars_cnt++ ;
	   vars[vars_cnt]=dcl_std_lib ;
                vars_cnt++ ;
	   vars[vars_cnt]=dcl_file_lib ;
                vars_cnt++ ;

       memset(libs, 0, sizeof(libs)) ;

           libs[0]=crn_path ;                                       /* Список библиотечных разделов */
            getcwd(crn_path, sizeof(crn_path)-1) ;

//          DCL.mDebug=RSS_dcl_debug ;

/*--------------------------------------------- Извлечение контекста */

          context=(RSS_Calc_Dcl_context *)*context_ptr ;
       if(context==NULL) {
                    
            context=(RSS_Calc_Dcl_context *)
                         calloc(1, sizeof(*context)) ;
         if(context==NULL) {
                               SEND_ERROR("Вычислитель> Недостаточно памяти для контекста") ;
                                                  return(-1) ;
                           }
                         }
/*---------------------------------- Внешние библиотеки и переменные */

       memset(sys_vars, 0, sizeof(sys_vars)) ;

                        i=0 ;
/*
       strcpy( sys_vars[i].name, "$contract") ;
               sys_vars[i].type=_DCL_CHAR_AREA ;
               sys_vars[i].addr= contract->contract ;
               sys_vars[i].size=strlen(contract->contract) ;
               sys_vars[i].buff= -1 ;
                        i++ ;
*/

      if(const_list!=NULL) {
         for(j=0 ; const_list[j].name[0] ; j++) {

           if(!stricmp(const_list[j].name, "$LIBRARY")) {

                         vars[vars_cnt]=(Dcl_decl *)const_list[j].ptr ;
                              vars_cnt++ ;
                                                        }
                                                }
                           }
/*-------------------------------- Выполнение операционной процедуры */

                     DCL.mWorkDir    =crn_path ;
                     DCL.mLibDirs    =libs ;
                     DCL.mVars       =vars ;

                     DCL.mProgramMem =program ;
                     DCL.mProgramFile= NULL ;

                     DCL.vProcess() ;

/*------------------------------------------------- Обработка ошибок */

      if(DCL.mError_code) {

            strcpy(error, DCL.vDecodeError(DCL.mError_code)) ;
//       CharToOem(error, error) ;

           sprintf(error, "File   :%s  Row:%d  Bad:<%s>\r\n"
                          "Error  :%d  %s\r\n"
                          "Details:%s\r\n",
                         (DCL.mError_file!=NULL)?DCL.mError_file:"NULL", 
                          DCL.mRow, 
                         (DCL.mError_position!=NULL)?DCL.mError_position:"NULL", 
                          DCL.mError_code, error, DCL.mError_details) ;

                                  return(-1) ;
                          }
/*---------------------------------------------- Обработка контекста */

          *context_ptr=context ;

/*-------------------------------------------------------------------*/

      return(0) ;
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Calc_Dcl::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Calc_Dcl::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         

//return ;

    if(path==NULL) {
                             path="dcl.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Процедура отладки вычислителя                 */

  void  RSS_Calc_Dcl_debug(int  frame)

{
   RSS_Calc_Dcl  module ;
           char  text[1024] ;


             sprintf(text, "Frame %d", frame) ;
       module.iDebug(text, NULL) ;

     if(_heapchk()!=_HEAPOK) module.iDebug("Frame Heap crash!!!", NULL) ;
     else                    module.iDebug("Frame Heap OK!!!", NULL) ;

}

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

#include "..\CCL_1\lang_std.h"
#include "..\CCL_1\ccl_1.h"

#include "C_Std.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
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

     static   RSS_Calc_Std  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 C_STD_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 C_STD_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/*								    */
/*		    	Буфер текста                                */

   static  char *RSS_Calc_Std_text    =NULL ;
   static  char *RSS_Calc_Std_text_cur=NULL ;

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

     RSS_Calc_Std::RSS_Calc_Std(void)

{
	   keyword="EmiStand" ;
    identification="Standart_calculator" ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

     RSS_Calc_Std::~RSS_Calc_Std(void)

{
}


/********************************************************************/
/*								    */
/*		        Стартовая разводка                          */

    void  RSS_Calc_Std::vStart(void)

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

  int  RSS_Calc_Std::vCalculate(            char  *expression_type,
                                            char  *expression, 
                            struct RSS_Parameter  *const_list,
                            struct RSS_Parameter  *var_list,
                                          double  *result,
                                            void **context_ptr, 
                                            char  *error       )

{
  RSS_Calc_Std_context *context ;
                   int  const_flag ;
                   int  exec_only ;
                   int  i ;
                   int  j ;

#define   _VARIABLES_MAX      20
#define   _FUNCTIONS_MAX      20

  static struct    L_table  ext_table[2] ;                /* Таблица списков переменных */
  static struct Ccl_object  variables[_VARIABLES_MAX] ;
  static struct L_function  functions[_FUNCTIONS_MAX] ;

#define  C     context->module_decl
#define        _BUFF_SIZE   64000

/*---------------------------------------- Идентификация вычислителя */

   if(stricmp(expression_type, "STD_EXPRESSION") &&
      stricmp(expression_type, "CLEAR"         )   )  return(1) ;

   if(context_ptr==NULL)  return(0) ;

//                                   iDebug("ExpressionType:", NULL) ;
//      if(expression_type!=NULL)    iDebug(expression_type, NULL) ;
//      else                         iDebug("NULL", NULL) ;
//                                   iDebug("Expression:", NULL) ;
//      if(expression!=NULL)         iDebug(expression, NULL) ;
//      else                    {
//                                   iDebug("NULL", NULL) ;
//                              }
//
//            lng_debug     = 1 ;
//            lng_user_debug=RSS_Calc_Std_debug ;

/*------------------------------------------------ Очистка контекста */

   if(!stricmp(expression_type, "CLEAR")) {

          context=(RSS_Calc_Std_context *)*context_ptr ;
       if(context==NULL)  return(0) ;

       if(C.ctrl_buff!=NULL)  free(C.ctrl_buff) ;
       if(C.culc_buff!=NULL)  free(C.culc_buff) ;
       if(C.int_vars !=NULL)  free(C.int_vars) ;
       if(C.work_vars!=NULL)  free(C.work_vars) ;
                              free(context) ;

                        *context_ptr=NULL ;

                           return(0) ;
                                          }   
/*--------------------------------------------- Извлечение контекста */

          context=(RSS_Calc_Std_context *)*context_ptr ;
       if(context==NULL) {
                    
            context=(RSS_Calc_Std_context *)
                         calloc(1, sizeof(*context)) ;
         if(context==NULL) {
                               SEND_ERROR("Вычислитель> Недостаточно памяти для контекста") ;
                                                  return(-1) ;
                           }
                         }

                    exec_only=context->compiled ;

/*--------------------------------------- Размещение рабочих буферов */

      if(!context->compiled) {

           C.ctrl_size=  _BUFF_SIZE ;
           C.ctrl_buff=(void *)calloc(1, C.ctrl_size) ;
           C.culc_size=  _BUFF_SIZE ;
           C.culc_buff=(void *)calloc(1, C.culc_size) ;

        if(C.ctrl_buff==NULL ||
           C.culc_buff==NULL   ) {
              SEND_ERROR("Вычислитель> Недостаточно памяти для кода") ;
                                                  return(-1) ;
                                 }

           C.int_vars_max =255 ;
           C.int_vars     =(struct L_variable *)calloc(C.int_vars_max, sizeof(*C.int_vars)) ;
           C.work_vars_max=255 ;
           C.work_vars    =(double *)calloc(C.work_vars_max, sizeof(*C.work_vars)) ;

        if(C.int_vars ==NULL ||
           C.work_vars==NULL   ) {
               SEND_ERROR("Вычислитель> Недостаточно памяти для переменных") ;
                                                  return(-1) ;
                                 }
                             }
/*--------------------------------------- Цикл адаптивной компиляции */

   if(!context->compiled) {                                         /* IF.1 */

    for(const_flag=1 ; const_flag>=0 ; const_flag--) {              /* CIRCLE.1 - Сначала пытаемся скомпилировать       */
                                                                    /*    только с константами, затем - и с переменными */
/*---------------------------------------------- Загрузка переменных */

                   memset(variables, 0, sizeof(variables)) ;

                                i=0 ;

      if(const_list!=NULL) {
         for(j=0 ; const_list[j].name[0] ; j++) {

           if(const_list[j].ptr!=NULL && const_flag==1)  continue ;

                     variables[i].name     =(unsigned char *)const_list[j].name ;
                     variables[i].func_flag=  0 ;
                     variables[i].type     =_CCL_DOUBLE ;

           if(const_list[j].ptr==NULL)
                     variables[i].addr=&const_list[j].value ;
           else      variables[i].addr= const_list[j].ptr  ;
                               i++ ;
                                                }
                           }

/*------------------------------------------------- Общая подготовка */

             ext_table[0].objects=variables ;
             ext_table[0].type   = _CCL_STD ;

	   C.ext_table           = ext_table ;                      /* Задаем список переменных */

	   C.ext_func            = functions ;                      /* Задаем список функций */
	   C.ext_func_cnt        =_FUNCTIONS_MAX ;
                            
/*------------------------------------------------------- Компиляция */
/*- - - - - - - - - - - - - - - - - Подготовка текста для компиляция */
     if(!stricmp(expression_type, "STD_EXPRESSION")) {

                RSS_Calc_Std_text_norm(expression) ;

            RSS_Calc_Std_text=(char *)calloc(1, strlen(expression)+1000) ;
         if(RSS_Calc_Std_text==NULL) {
             SEND_ERROR("Вычислитель> Недостаточно памяти для текста") ;
                                                  return(-1) ;
                                     }

            sprintf(RSS_Calc_Std_text, "ZZZ=%s\n"
                                       "RETURN(ZZZ)", expression) ; 
                                                     }
     else                                            {
            RSS_Calc_Std_text=strdup(expression) ;
         if(RSS_Calc_Std_text==NULL) {
             SEND_ERROR("Вычислитель> Недостаточно памяти для текста") ;
                                                  return(-1) ;
                                     }
                                                     }

                        RSS_Calc_Std_text_cur=NULL ;
             C.get_text=RSS_Calc_Std_get_text ;                     /* Уст.п/п построчной передачи текста */
					                            /*         NULL - конец текста        */
/*- - - - - - - - - - - - - - - - - - - - - -  Собственно компиляция */
                L_compile(&context->module_decl) ;

                    free(RSS_Calc_Std_text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обработка ошибок */
     if(!stricmp(expression_type, "STD_EXPRESSION")) {

      if(lng_error)  sprintf(error, "Error: %s \nBad:<%s>",
		 	          RSS_Calc_Std_error(lng_error), lng_bad) ;

                                                     }
/*
   if(lng_error  && var_list!=NULL) { 
                   sprintf(message, "Error: %s \nRow: %d \nBad:<%s>",
		 	                 Get_CCL_error(lng_error), lng_row, lng_bad) ;
              Show_box_add(message) ;
                                    }

   for(i=0 ; i<lng_warn_cnt ; i++) {
     sprintf(message, "Warning in row %d : %s",
                                           lng_warn[i].row,  
		           Get_CCL_warning(lng_warn[i].code) ) ;
         Show_box_add(message) ;

                                   }
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*--------------------------------------- Цикл адаптивной компиляции */

           if(lng_error==0) {  context->compiled=1 ;
                                       break ;        }

                                                     }              /* CONTINUE.1 */                                                    
                          }                                         /* END.1 */
/*------------------------------------------------------- Вычисление */

    if(!lng_error) {

        *result=L_execute(&context->module_decl, NULL, NULL, 0) ;

                       if(exec_only)  return(0) ;
                   }
/*---------------------------------------------- Обработка контекста */
/*- - - - - - - - - - - - - - - - - - - - - - Если ошибка компиляции */
        if(lng_error ) {

                                free(C.ctrl_buff) ;
                                free(C.culc_buff) ;
                                free(C.int_vars) ;
                                free(C.work_vars) ;
                                free(context) ;

                                     context=NULL ;
                       }
/*- - - - - - - - - - - - - - - - - - - - Если константное выражение */
   else if(const_flag) {

                                free(C.ctrl_buff) ;
                                free(C.culc_buff) ;
                                free(C.int_vars) ;
                                free(C.work_vars) ;
                                free(context) ;

                                     context=NULL ;
                       }
/*- - - - - - - - - - - - - - - - - - - -  Если переменное выражение */
   else                {

            C.ctrl_buff=                     realloc(C.ctrl_buff,
                                                     C.ctrl_used ) ;
            C.culc_buff=                     realloc(C.culc_buff,  
                                                     C.culc_used ) ;
//          C.int_vars =(struct L_variable *)realloc(C.int_vars,   
//                                                   C.int_vars_cnt*sizeof(*C.int_vars)) ;
//          C.work_vars=           (double *)realloc(C.work_vars, 
//                                                   C.work_vars_cnt*sizeof(*C.work_vars)) ; ;

                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               *context_ptr=context ;

/*-------------------------------------------------------------------*/

   if(lng_error)  return(-1) ;
                  return( 0) ;  
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Calc_Std::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Calc_Std::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         

//return ;

    if(path==NULL) {
                             path="c_std.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}


/********************************************************************/
/*								    */
/*                 Нормализация строки выражения                    */

  void  RSS_Calc_Std_text_norm(char *text)

{
     char *tmp ;

/*----------------------------------- Замена спецсимволов на пробелы */

      for(tmp=text ; *tmp ; tmp++)
        if(*tmp=='\t' ||
           *tmp=='\r' ||
           *tmp=='\n'   )  *tmp=' ' ;

/*-------------------------------------------------- Ужатие пробелов */

     while(1) {
                    tmp=strchr(text, ' ') ;
                 if(tmp==NULL)  break ;

                    strcpy(tmp, tmp+1) ;
              }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*         Процедура построчной выдачи текста макро-компилятору      */
/*                                                                   */
/*  При каждом обращении возвращает указатель на одну строку текста, */
/*   а при его завершении - NULL указатель                           */

  char *RSS_Calc_Std_get_text(void)

{
  static char *next ;

  if(RSS_Calc_Std_text_cur==NULL)  
          RSS_Calc_Std_text_cur=RSS_Calc_Std_text ;
  else    RSS_Calc_Std_text_cur=  next ;

  if(RSS_Calc_Std_text_cur==NULL)  return(NULL) ;

          next=strchr(RSS_Calc_Std_text_cur, '\n') ;
       if(next!=NULL) {
                         *next=0 ;
                          next++ ;
                      }
   
  return(RSS_Calc_Std_text_cur) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Выдача текста ошибки по коду                   */
/*                    Выдача текста предупреждения по коду           */

  char *RSS_Calc_Std_error(int  code) 

{
  static struct {  int  code ;
                  char *text ;  }  error_messages[]={
   { _EL_GET_TEXT    , "Не задана процедура подачи текста"                                  },
   { _EL_CTRL_BUFF   , "Переполнение буфера управл.кода"                                    },
   { _EL_CULC_BUFF   , "Переполнение буфера счетного кода"                                  },
   { _EL_CYR_CHAR    , "'Русский' символ в тексте"                                          },
   { _EL_MARK_BAD    , "Некорректная или слишком длинная метка"                             },
   { _EL_MARK_DBL    , "Повторная метка"                                                    },
   { _EL_MARK_UNK    , "Неизвестная метка"                                                  },
   { _EL_END_MARK    , "Оператор END содержит метку"                                        },
   { _EL_END_FILE    , "Отсутствует оператор END"                                           },
   { _EL_NAME_LEN    , "Слишком длинное имя"                                                },
   { _EL_NAME_EMPTY  , "Пустое имя"                                                         },
   { _EL_RET_OUT     , "В тексте отсутствует RETURN"                                        },
   { _EL_RET_FMT     , "Некорректный формат оператора RETURN"                               },
   { _EL_RET_NAME    , "Парам. опер. RETURN - выражение или некорр.имя"                     },
   { _EL_RET_UNK     , "Имя RETURN-переменной неизвестно"                                   },
   { _EL_IF_FMT      , "Некорректный формат оператора IF"                                   },
   { _EL_ELSE_MARK   , "Оператор ELSE содержит метку"                                       },
   { _EL_ELSE_UNK    , "ELSE без IF"                                                        },
   { _EL_BLOCK_UNK   , "Попытка закрыть неоткрытый блок"                                    },
   { _EL_UNCLOSE     , "'Незакрытые' операторы IF или DO"                                   },
   { _EL_DO_FMT      , "Некорректный формат оператора DO"                                   },
   { _EL_EXT_CRC     , "BREAK или CONTINUE без цикла"                                       },
   { _EL_TEXT_OPEN   , "Открытая текстовая константа"                                       },
   { _EL_INV_CHAR    , "Некорректная Escape-последовательность"                             },
   { _EL_DECL_FMT    , "Некорректный формат описания"                                       },
   { _EL_DECL_SIZE   , "Некорректный размер переменной"                                     },
   { _EL_NAME_USED   , "Повторное использование имени"                                      },
   { _EL_ENTRY_UNK   , "Неуказанный адрес ENTRY-входа"                                      },
   { _EL_FUNC_BEFORE , "Перед функцией непонятный символ"                                   },
   { _EL_FUNC_FMT    , "Некоректный формат вызова функции"                                  },
   { _EL_FUNC_SINTAX , "Неизвестный или недопустимый параметр"                              },
   { _EL_FUNC_UNK    , "Неисвестное имя функции"                                            },
   { _EL_OPEN        , "Лишние открывающие скобки"                                          },
   { _EL_CLOSE       , "Лишние закрывающие скобки"                                          },
   { _EL_EMPTY       , "Пустое выражение (подвыражение в скобках)"                          },
   { _EL_NAME_UNK    , "Неизвестное имя переменной"                                         },
   { _EL_NAME_SINTAX , "Нкорректное имя переменной"                                         },
   { _EL_INDEX       , "Незакрытое индексное выражение"                                     },
   { _EL_SINTAX1     , "Ошибка синтаксиса 1"                                                },
   { _EL_SINTAX2     , "Ошибка синтаксиса 2"                                                },
   { _EL_SINTAX3     , "Ошибка синтаксиса 3"                                                },
   { _EL_SINTAX4     , "Ошибка синтаксиса 4"                                                },
   { _EL_SINTAX5     , "Ошибка синтаксиса 5"                                                },
   { _EL_STD_OUT     , "Вызов функций стандарта CCL или DCL"                                },
   { _EL_VARS_OUT    , "Число переменных, заданных через lng_vars не может быть более 255"  },
   { _EL_FUNC_OUT    , "Число функций, заданных через lng_func не может быть более 255"     },
   { _EL_LEN_MAX     , "Слишком длинная исходная строка"                                    },
   { _EL_IFDO_MAX    , "Слишком глубокая вложенность операторов IF и DO"                    },
   { _EL_INT_VAR_MAX , "Слишком много внутренних переменных"                                },
   { _EL_WRK_VAR_MAX , "Слишком много рабочих ячеек в строке"                               },
   { _EL_PARS_MAX    , "Слишком много параметров"                                           },
   { _EL_TEXT_MAX    , "Слишком много строчных констант"                                    },
   { _EL_INDEX_MAX   , "Слишком много индексов"                                             },
   { _EL_AREA_MAX    , "Переполнение буфера внутренних массивов"                            },
   { _EL_ENTRY_MAX   , "Переполнение списка входов"                                         },
   { _EL_GOTO_OVR    , "Слишком много меток или GOTO-переходов"                             },
   { _EL_IFDO_OVR    , "Переполнение IFDO-стека"                                            },
   { _EL_UNK_ENTRY   , "Неизвестный вход модуля"                                            },
   {   0, NULL  } 
                                                    } ;

 static char unknown_text[1024] ;
         int  i ;


      for(i=0 ; error_messages[i].code ; i++)
        if(code==error_messages[i].code)  return(error_messages[i].text) ;

       sprintf(unknown_text, "Неизвестная ошибка %d", code) ;

  return(unknown_text) ;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  char *RSS_Calc_Std_warning(int  code) 

{
  static struct {  int  code ;
                  char *text ;  }  error_messages[]={
                   { _WL_EXT_CHNG  , "Попытка изменения внешней переменной"    },
                   { _WL_FUNC_PARS , "Несоответствии числа параметров функции" },
                   {   0, NULL  } 
                                                    } ;

 static char unknown_text[1024] ;
         int  i ;


      for(i=0 ; error_messages[i].code ; i++)
        if(code==error_messages[i].code)  return(error_messages[i].text) ;

       sprintf(unknown_text, "Неизвестное предупреждение %d", code) ;

  return(unknown_text) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Процедура отладки вычислителя                 */

  void  RSS_Calc_Std_debug(int  frame)

{
   RSS_Calc_Std  module ;
           char  text[1024] ;


             sprintf(text, "Frame %d", frame) ;
       module.iDebug(text, NULL) ;

     if(_heapchk()!=_HEAPOK) module.iDebug("Frame Heap crash!!!", NULL) ;
     else                    module.iDebug("Frame Heap OK!!!", NULL) ;

}

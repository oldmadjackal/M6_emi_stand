/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                       ВНЕШНИЙ ИНТЕРФЕЙС                           */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include <memory.h>
#ifndef UNIX
#include <conio.h>
#endif

#define  _DCL_MAIN_
#define   DCL_INSIDE

#include "dcl.h"

/********************************************************************/
/*								    */
/*		       Статические переменные			    */

  class Lang_DCL *Lang_DCL::ActiveInstance ;


/*********************************************************************/
/*                                                                   */
/*                          Конструктор                              */

     Lang_DCL::Lang_DCL(void)

{
             mExt_lib     =NULL ;

             nInit_flag   =  0 ;

             nX_types     =  0 ;
             nX_cnt       =  0 ;

             nAll_vars    =NULL ;

             nCall_list   =NULL ;
             nCall_stack  =NULL ;

             nT_blocks    =NULL ;
             nT_blocks_cnt=  0 ;

             nPars_cnt    =  0 ;
             nPars_cnt_all=  0 ;
             nTransit     =NULL ;
             nTransit_cnt =  0 ;
             nTransit_num =  0 ;
     memset(&nResult, 0, sizeof(nResult)) ;
     memset(&nReturn, 0, sizeof(nReturn)) ;
             nRet_data    =NULL ;

     memset(&nVoid, 0, sizeof(nVoid)) ;
     memset(&nTmp , 0, sizeof(nTmp )) ;

    iIgnoreSizeCorrection =  0 ;

           ActiveInstance = this ;
}


/*********************************************************************/
/*                                                                   */
/*                          Деструктор                               */

     Lang_DCL::~Lang_DCL(void)

{
            vFree() ;
}


/*********************************************************************/
/*                                                                   */
/*                       Освобождение ресурсов                       */

    void  Lang_DCL::vFree(void)

{
            iCall_free() ;
             iMem_free() ;
}


/*********************************************************************/
/*                                                                   */
/*                       Исполнение программы                        */

    void  Lang_DCL::vProcess(void)

{
             nSeg_row   = 0 ;
             nSeg_start = 0 ;

             nInit_flag = 0 ;

   if(mProgramFile)  Interpritator(_DCL_FILE,   mProgramFile, (Dcl_decl **)mVars) ;
   if(mProgramMem )  Interpritator(_DCL_MEMORY, mProgramMem,  (Dcl_decl **)mVars) ;
}


/*********************************************************************/
/*                                                                   */
/*              Получение текста ошибки по коду                      */

    char *Lang_DCL::vDecodeError(int  code)

{
 static struct {
                  int  code ;
                 char *text ;
               } errors[]={
                            {  _DCLE_FILE_OPEN     , "Ошибка открытия файла"                                },
                            {  _DCLE_STRING_LEN    , "Слишком длинная строка"                               },
                            {  _DCLE_NOCLOSE_STRING, "Незакрытые кавычки"                                   },
                            {  _DCLE_MARK_SIMBOL   , "Некорректная метка"                                   },
                            {  _DCLE_MARK_LEN      , "Слишком длинная метка"                                },
                            {  _DCLE_IF_CLOSE      , "Потеряна закр.скобка оператора IF"                    },
                            {  _DCLE_FOR_CLOSE     , "Потеряна закр.скобка заголовка цикла"                 },
                            {  _DCLE_RETURN_CLOSE  , "Потеряна закр.скобка оператора RETURN"                },
                            {  _DCLE_FOR_TTL_SEL   , "Потерян рассекатель частей заголовка"                 },
                            {  _DCLE_FREE_OPEN     , "Открытие 'свободного' блока"                          },
                            {  _DCLE_FREE_CLOSE    , "Закрытие неоткрытого блока"                           },
                            {  _DCLE_FREE_CONTINUE , "'Свободный' оператор CONTINUE"                        },
                            {  _DCLE_FREE_BREAK    , "'Свободный' оператор BREAK"                           },
                            {  _DCLE_FREE_ELSE     , "ELSE без IF-а"                                        },
                            {  _DCLE_OPEN          , "Незакрытое выражение в скобках"                       },
                            {  _DCLE_CLOSE         , "Лишняя закрывающая скобка"                            },
                            {  _DCLE_UNKNOWN_NAME  , "Неизвестная переменная или процедура"                 },
                            {  _DCLE_SINTAX_NAME   , "Некорректный синтакс имени"                           },
                            {  _DCLE_NOINDEX       , "Пустое или незакрытое индексное выражение"            },
                            {  _DCLE_INDEXED_VAR   , "Индексирование простой переменной"                    },
                            {  _DCLE_INDEX_TYPE    , "Некорректный тип индекса"                             },
                            {  _DCLE_INDEX_FRG     , "Фрагментарное индексирование нестрочного объекта"     },
                            {  _DCLE_INVALID_CHR   , "Некорректный символ в символьной константе"           },
                            {  _DCLE_BAD_OPERATION , "Некорректная операция"                                },
                            {  _DCLE_BAD_LEFT      , "Некорректный левый операнд"                           },
                            {  _DCLE_BAD_RIGHT     , "Некорректный правый операнд"                          },
                            {  _DCLE_EMPTY_LEFT    , "Отсутствует левый операнд"                            },
                            {  _DCLE_EMPTY_RIGHT   , "Отсутствует правый операнд"                           },
                            {  _DCLE_EMPTY_BOTH    , "Операция без операндов"                               },
                            {  _DCLE_BINARY_OPER   , "Бинарная операция с одним операндом"                  },
                            {  _DCLE_UNARY_OPER    , "Унарная операция с двумя операндами"                  },
                            {  _DCLE_PREFIX_OPER   , "Операция должна быть префиксной"                      },
                            {  _DCLE_EMPTY_DEST    , "При операции отсутствует 'приемник'"                  },
                            {  _DCLE_DIF_TYPE      , "Тип 'приемника' не соотв.типу результата"             },
                            {  _DCLE_CONST_DEST    , "'Приемник' является немодифицируемым"                 },
                            {  _DCLE_PROTOTYPE     , "Тип параметра не соответствует прототипу"             },
                            {  _DCLE_TYPEDEF_ELEM  , "Неизвестный элемент комплексного объекта"             },
                            {  _DCLE_TYPEDEF_METH  , "Неизвестный метод комплексного объекта"               },
                            {  _DCLE_TYPEDEF_SORT  , "Поле сортировки имеет различный тип"                  },
                            {  _DCLE_TYPEDEF_AREA  , "Динамический массив в комплексном обьекте"            },
                            {  _DCLE_TBLOCK_CLOSE  , "Не закрытый текстовой блок"                           },
                            {  _DCLE_TBLOCK_SIZE   , "Слишком большой текстовой блок"                       },

                            {  _DCLE_PRAGMA_UNK    , "Неизвестная прагма"                                   },

                            {  _DCLE_BAD_PREF      , "Некорректный префикс"                                 },
                            {  _DCLE_DBL_MOD       , "Некорректное описание"                                },
                            {  _DCLE_NO_TYPE       , "Описание не задает базовый тип"                       },
                            {  _DCLE_PTR_AREA      , "Массив указателей запрещен"                           },
                            {  _DCLE_BLOCK_DECL    , "Обьект BLOCK должен быть простым массивом"            },
                            {  _DCLE_TYPEDEF_IN    , "Вложенный оператор typedef"                           },
                            {  _DCLE_TYPEDEF_STR   , "Некорректная структура оператора typedef"             },
                            {  _DCLE_TYPEDEF_RDF   , "Переопределение типа"                                 },
                            {  _DCLE_TYPEDEF_NAME  , "Имя типа не соответствует соглашениям"                },
                            {  _DCLE_NAME_INUSE    , "Имя переменной уже используется"                      },

                            {  _DCLE_CALL_INSIDE   , "Внутри-процедурная ошибка"                            },

                            {  _DCLE_USER_DEFINED  , "Ошибка контекста исполнения"                          },

                            {  _DCLE_ELIB_INIT     , "Ошибка иниц.системы внешних библиотек"                },
                            {  _DCLE_ELIB_DECL     , "Некорректное описание обьекта во внешней библиотеке"  },
                            {  _DCLE_ELIB_LOAD     , "Неудачная загрузка обьекта из внешней библиотеке"     },
                            {  _DCLE_MEMORY        , "Недостаток памяти"                                    },
                            {  _DCLE_MEMORY_VT     , "Недостаток памяти для заголовков переменных"          },
                            {  _DCLE_MEMORY_VA     , "Недостаток памяти для переменных"                     },
                            {  _DCLE_LOAD_LIST     , "Слишко много подгружаемых переменных"                 },
                            {  _DCLE_STACK_OVR     , "Переполнение стека"                                   },
                            {  _DCLE_STACK_UND     , "Извлечение пустого стека"                             },
                            {  _DCLE_STACK_MEM     , "Недостаточно памяти для стека"                        },
                            {  _DCLE_UNKNOWN       , "Имени нет в списках"                                  },
                            {  _DCLE_EMPTY         , "Пустое простое выражение"                             },
                            {  _DCLE_WORK_FAULT    , "Сбой адресации заголовка рабочей ячейки"              },
                            { 0, NULL }
                         };
          int  i ;      

    for(i=0 ; errors[i].text!=NULL ; i++)
      if(errors[i].code==code)  return(errors[i].text) ;

    return("Неизвестная ошибка") ;
}
#ifndef  _DCL_H_
#define  _DCL_H_

#ifndef UNIX
#pragma warning( 3 : 4701 )
#endif

#include "stddef.h"
#include "stdio.h"

//#include "..\\Lang_common\\lang_std.h"

#ifdef  _DCL_MAIN_
#define  _EXTERNAL
#else
#define  _EXTERNAL  extern
#endif

/*---------------------------------------------- Привязка к системам */

#ifdef UNIX

#define              stricmp  strcasecmp
#define              memicmp  memcmp
#define                _stat  stat
#define  GetCurrentProcessId  getpid
#else

#endif

/*--------------------------------- Некоторые интерфейсные мнемоники */

#ifndef _DCL_LOAD
#define _DCL_LOAD  (void *)0x0000fff1L  /* Вызов DCL-процедуры */
#endif

#ifndef _DCL_STD
#define _DCL_STD          3             /* Обьекты в стандарте DCL */
#endif

#ifndef _DCL_CALL
#define _DCL_CALL         1             /* Стандарт параметров  DCL */
#endif

#ifndef _DCL_METHOD
#define _DCL_METHOD       2             /* Стандарт методов  DCL */
#endif

/*---------------------------------------------- Параметры генерации */

#define    _DCL_NAME_SIZE       32     /* Максимальный размер имени */
#define    _DCL_PARS_MAX       100     /* Число параметров п/п */
#define    _DCL_IFFOR_MAX       20     /* Число число вложенных IF и FOR операторов */
#define    _DCL_OPER_MAX        10     /* Число подоператоров */


#ifdef  DCL_INSIDE

#define    _VERSION     "2.00.001 2016/05/19"

#define    _NAME_SIZE    _DCL_NAME_SIZE   
#define    _PARS_MAX     _DCL_PARS_MAX
#define   _IFFOR_MAX     _DCL_IFFOR_MAX
#define    _OPER_MAX     _DCL_OPER_MAX

#define    _CALL_MAX       500     /* Число подпрограмм */
#define   _STACK_MAX        10     /* Размер стека вызовов */

#define   _TITLE_PAGE     1000     /* Размер страницы заголовков */
#define     _DYN_CHUNK   10000     /* Минимальная порция наращивания для динамических массивов */

#define  _TBLOCK_SIZE    64000     /* Максимальный размер текстового блока */

#define    _BUFF_SIZE     2048

#endif

/*--------------------------------- Описатели обьектов стандарта DCL */

typedef struct {                                 /* Описание в стандарте DCL */
	        unsigned int  type ;                  /* Тип переменной/процедуры */
	        unsigned int  func_flag ;             /* Флаг процедуры(тип функции) */
	        unsigned int  work_nmb ;              /* Номер рабочей ячейки (0 - не рабочая)  */
				                      /*  или "адрес" в таблице DCL-модулей     */
				                      /*  или спецификатор комплексного объекта */
                         int  local_idx ;             /* Индекс зоны локала: 0 - глобальная переменная */      
			char  name[_DCL_NAME_SIZE] ;  /* Имя переменной/процедуры */
			void *addr ;                  /* Адрес переменной/процедуры */
			char *prototype ;             /* Типы параметров процедуры или признак динамической переменной "D" */
			 int  size ;                  /* Размер переменной */
			 int  buff ;                  /* Общая вместимость, если -1 - переменная является READ ONLY*/
	       } Dcl_decl ;

/*  Тип обьекта имеет следующую структуру:            */
/*                                                    */
/*        0...7  -  Базовый тип                       */
/*        8...b  -  Уточняющий типа                   */

				/* Базовый тип - НИКОГДА НЕ ТРОГАТЬ !!! */
#define  _DCLT_DGT_VAL   0x0002   /* Число */
#define  _DCLT_DGT_AREA  0x0004   /* Массив чисел или указатель на число */
#define  _DCLT_DGT_PTR   0x0005   /* Указатель на массив чисел */
#define  _DCLT_CHR_AREA  0x0008   /* Строка(массив символов) */
#define  _DCLT_CHR_PTR   0x0009   /* Указатель на строку */
#define  _DCLT_NULL_PTR  0x0010   /* NULL - указатель */
#define  _DCLT_XTP_OBJ   0x0020   /* Объект комплексного типа */
#define  _DCLT_XTP_REC   0x0040   /* Запись объекта комплексного типа */

				/* Уточняющий тип */
#define   _DCLT_SHORT    0x0100   /*            short       */
#define   _DCLT_USHORT   0x0200   /*   unsigned short       */
#define   _DCLT_LONG     0x0300   /*            long        */
#define   _DCLT_ULONG    0x0400   /*   unsigned long        */
#define   _DCLT_FLOAT    0x0500   /*            float       */
#define   _DCLT_DOUBLE   0x0000   /*            double      */
#define   _DCLT_UNKNOWN  0xff00

			      /* Полные типы */
#define   _DCL_SHORT       (_DCLT_DGT_VAL | _DCLT_SHORT )
#define   _DCL_USHORT      (_DCLT_DGT_VAL | _DCLT_USHORT)
#define   _DCL_LONG        (_DCLT_DGT_VAL | _DCLT_LONG  )
#define   _DCL_ULONG       (_DCLT_DGT_VAL | _DCLT_ULONG )
#define   _DCL_FLOAT       (_DCLT_DGT_VAL | _DCLT_FLOAT )
#define   _DCL_DOUBLE      (_DCLT_DGT_VAL | _DCLT_DOUBLE)
#define   _DCL_CHAR_AREA    _DCLT_CHR_AREA
#define   _DCL_CHAR_PTR     _DCLT_CHR_PTR
#define   _DCL_SHORT_PTR   (_DCLT_DGT_PTR | _DCLT_SHORT )
#define   _DCL_USHORT_PTR  (_DCLT_DGT_PTR | _DCLT_USHORT)
#define   _DCL_LONG_PTR    (_DCLT_DGT_PTR | _DCLT_LONG  )
#define   _DCL_ULONG_PTR   (_DCLT_DGT_PTR | _DCLT_ULONG )
#define   _DCL_FLOAT_PTR   (_DCLT_DGT_PTR | _DCLT_FLOAT )
#define   _DCL_DOUBLE_PTR  (_DCLT_DGT_PTR | _DCLT_DOUBLE)

#define   _DCLT_BASE_MASK    0x00ff
#define   _DCLT_MODE_MASK    0xff00

#define   DCL_WORK_NUM(m_idx, p_idx) (m_idx<<16 | p_idx)
#define   DCL_WORK2M(n)              (n>>16)
#define   DCL_WORK2P(n)              (n & 0x00ffff)

#define  _DCL_MAIN_P                  0x00fff

#define _DCL_LOAD  (void *)0x0000fff1L  /* Вызов DCL-процедуры */

/*  Спецификаторы комплексного объекта: */

#define  _DCL_MARKED_ONLY      0x0001    /* Использовать режим отбора "MARKED ONLY" */
#define  _DCL_XTRACE           0x0002    /* Подлежит слядящей коррекции */

/*---------------------------------------------- Некоторые структуры */

typedef struct {                   /* Описание дешифровки мнемоник */
                    char *mnemo ;   /* Мнемоника */
                     int  code ;    /* Код */
               } Dcl_decode ;

typedef struct {                            /* Описание комплексного типа */
		   char  name[_DCL_NAME_SIZE] ;  /* Мнемоника комплексного типа */
                    int  size ;              /* Общий размер типа */
               Dcl_decl *list ;              /* Список компонентов */
                    int  list_cnt ;
	       } Dcl_complex_type ;

typedef struct {                            /* Заголовок записи комплексного типа */
                     void *next_record ;     /* Ссылка на следующую запись */
                 Dcl_decl *elems ;           /* Список элементов записи */
                      int  elems_cnt ;
                      int  size ;            /* Общий размер записи */
                      int  find_mark ;       /* Метка метода FIND */
                      int  exec_mark ;       /* Системная метка отработки */
                 Dcl_decl  fld_name ;        /* Ссылка на имя текущего поля - $name */
                 Dcl_decl  fld_value ;       /* Ссылка на значение текущего поля - $value */
                      int  fld_idx ;
               } Dcl_complex_record ;

typedef struct Dcl_call {                       /* Описание вызова DCL-процедуры */
                 Dcl_decl  decl ;                /* Описание как языкового объекта */ 
		     char  name[_DCL_NAME_SIZE] ;    /* Имя обьекта */
		     char *path ;                /* Имя файла */
		      int  mem_flag ;            /* Флаг размещения в памяти */
		     long  file_addr ;           /* Файловый адрес обьекта */
		     char *mem_addr ;            /* Адрес обьекта в памяти */
          struct Dcl_call *proc_list ;           /* Список локальных и виртуальных процедур */
                      int  proc_cnt ;
                      int  v_flag ;              /* Флаг виртуальной процедуры */
               } Dcl_call ;

typedef struct {                                /* Описание TEXT-блока */
		     char  name[_DCL_NAME_SIZE] ;    /* Имя TEXT-блока */
		     char *data ;                /* Данные */
               } Dcl_tblock ;

typedef struct {                                /* Описание стека IF-FOR структур */ 
		     char  if_for_flag ;         /* Флаг типа структуры: 1-FOR, 0-IF */
		     char  else_flag ;           /* Флаг ветви IF структуры: 1-ELSE, 0-IF */
		     char  block_flag ;          /* Флаг работы с блочной структурой */
		     char  pass_sts ;            /* Тип прохода структуры */
		     long  addr ;                /* Начало цикла - адрес строки */
		      int  oper ;                /* Начало цикла - оператор */
		      int  row ;                 /* Начало цикла - строка */
               } Dcl_if_for ;

typedef struct {                               /* Описание системы подстрок и операторов */
		     char *beg ;                /* Начало оператора */
		     char *end ;                /* Окончание оператора */
		      int  type ;               /* Тип оператора */
               }  Dcl_oper ;

/*--------------------------------------- Описание класса Lang_iface */

  class Lang_iface {

   public : 
                     int   mDebug_flag  ;                 /* Флаг отладки */
                     int (*mDebug)(void) ;                /* Пользовательская процедура отладки */
                     int   mDebug_trace  ;                /* Флаг трассировки */
                    char   mDebug_path[FILENAME_MAX] ;    /* Путь к файлу лога */

                     int   mError_code ;                  /* Код ошибки */
                    char   mError_details[1024] ;         /* Описание ошибки */
                    char  *mError_file ;                  /* Файл с ошибкой */
                    char  *mError_position ;              /* Указатель "плохого" места */

                    char   mProgram_name[FILENAME_MAX] ;  /* Название программы */
	             int   mFile_flag ;                   /* Флаг размещения программы в файле */
	             int   mFile_hdl ;                    /* Указатель файла программы */
                    char  *mFile_mem ;                    /* Буфер программы в памяти */
                     int   mRow ;                         /* Счетчик строк программы */
                    long   mByte_cnt ;                    /* Счетчик байтов ввода */

                    char  *mWorkDir ;                     /* Рабочий раздел */
                    char **mLibDirs ;                     /* Каталоги библиотек (NULL-терминированные) */

                    void  *mVars ;                        /* Ссылка на списки программных объектов */

                    char  *mProgramFile ;                 /* Путь программы */
                    char  *mProgramMem ;                  /* Буфер с программой в памяти */

            virtual void   vFree   (void){} ;             /* Освобождение ресурсов */
            virtual void   vProcess(void){} ;             /* Исполнение программы */
            virtual char  *vDecodeError(int){             /* Получение текста ошибки по коду */
                                             return(NULL) ;
                                            } ;          

                           Lang_iface(){                  /* Конструктор */  
                                          mError_file    =NULL ;
                                          mError_code    =  0 ; 
                                          mRow           =  0 ;
                                          mError_position=NULL ;
                                          mWorkDir       =NULL ;
                                          mLibDirs       =NULL ;
                                          mDebug_flag    =  0 ;
                                          mDebug         =NULL ;
                                          mDebug_trace   =  0 ;
                                         *mDebug_path    =  0 ;
                                       } ;               

                          ~Lang_iface(){} ;               /* Деструктор */
                 } ;
/*---------------------------------------------- Описание класса DCL */

  class Lang_DCL : public Lang_iface {

   public : 

              virtual void   vFree       (void) ;   /* Освобождение ресурсов */
              virtual void   vProcess    (void) ;   /* Исполнение программы */
              virtual char  *vDecodeError(int) ;    /* Получение текста ошибки по коду */

	                     Lang_DCL() ;           /* Конструктор */
	                    ~Lang_DCL() ;           /* Деструктор */

    static  class Lang_DCL  *ActiveInstance ;
 
   public : 

    /* Первым параметром пользовательской функции mExt_lib является */
    /* тип подгружаемого обьекта, а вторым - его символьное имя.    */
    /* Если обьект найден - возвращается указатель на его описание, */
    /* а если нет - NULL.                                           */
             Dcl_decl *(*mExt_lib)(int, char *) ;     /* Функция внешней библиотеки */
#define                  _DCL_LOAD_VAR    1            /* Подгрузить переменную */
#define                  _DCL_LOAD_FNC    2            /* Подгрузить функцию */

   public : 
                  int   nInit_flag ;                  /* Флаг инициализации */ 

     Dcl_complex_type  *nX_types ;                    /* Описание комплексных типов */
                  int   nX_cnt ;

             Dcl_decl  *nPars_table[_DCL_PARS_MAX] ;      /* Таблица параметров */
                  int   nPars_cnt ;                   /* Счетчик параметров текущего вызова */
                  int   nPars_cnt_all ;               /* Суммарный счетчик параметров */
             Dcl_decl  *nTransit ;                    /* Таблица транзитных параметров */
                  int   nTransit_cnt ;                /* Число транзитных параметров */
                  int   nTransit_num ;                /* Номер транзитного параметра */
             Dcl_decl   nReturn ;                     /* Описание возврата */
                 void  *nRet_data ;                   /* Буфер данных возврата */

             Dcl_call   nCall_main ;                  /* Вызовов main-процедуры */
             Dcl_call  *nCall_list ;                  /* Кэш-лист модулей */
                  int   nCall_list_cnt ;
             Dcl_call **nCall_stack ;                 /* Стек вызовов процедур */
                  int   nCall_stack_ptr ;             /* Указатель стека вызовов процедур */
                  int   nLocal ;                      /* Индекс локала */

             Dcl_decl **nVars ;                       /* Описание внешних(?) переменных и процедур */
                  int   nBuff_max ;                   /* Максимальный размер буфера переменной */
             Dcl_decl   nResult ;                     /* Описание результата */
  	          int   nResult_dst ;                 /* Ячейка размещения результата */
                  int   nIf_ind ;                     /* Индикатор условия */
               double   nDgt_val ;                    /* Числовое значение */
	         char  *nChr_pnt ;                    /* 'Нужный' фрагмент строки */
//               char   nConv[128] ;                  /* Буфер преобразований */
//           Dcl_decl  *nTarget  ;                    /* Описание нужной переменной и процедуры */
             Dcl_decl   nTmp ;                        /* Псевдо-ячейка */
             Dcl_decl   nVoid ;

             Dcl_decl **nAll_vars ;                   /* Общая таблица списков переменных */
             Dcl_decl  *nInt_page ;                   /* Страница заголовков внутр.переменных */
                  int   nInt_cnt ;                    /* Счетчик заголовков внутреннух переменных */
             Dcl_decl  *nWork_page ;                  /* Страница заголовков рабочих переменных */
                 void **nWork_mem ;                   /* Страница выделения памяти рабочих переменных */
                  int   nWork_cnt ;                   /* Счетчик рабочих переменных */  
                  int   nFix_flag ;                   /* Флаг сохранения систем интерпритатора без очистки после завершения программы */
           Dcl_tblock  *nT_blocks ;                   /* Список текстовых блоков */
                  int   nT_blocks_cnt ;

           Dcl_if_for   nId_stk[_DCL_IFFOR_MAX] ;     /* Стек IF-DO операторов */
                  int   nId_stk_cnt ;                 /* Указатель стека IF-DO операторов */
                  int   nId_num ;                     /* Указатель "крайнего блока" IF_DO стека */
                  int   nNext_flag ;                  /* Флаг повторного прохода цикла */

                  int   nSubrow_cnt ;                 /* Счетчик подстрок */
             Dcl_oper   nOper[_DCL_OPER_MAX] ;        /* Массив описания операторов */
                  int   nOper_cnt ;                   /* Счетчик операторов */
                  int   nOper_crn ;                   /* Номер текущей оператора */
                  int   nOper_next ;                  /* Номер начального оператора при возврате на начало цикла */

		 long   nSeg_start ;                  /* Начальный адрес программного сегмента */
		  int   nSeg_row  ;                   /* Начальная строка программного сегмента */
                  int   nPass_sts ;                   /* Статус прохода */

                  int   iIgnoreSizeCorrection ;       /* Запрет проведения коррекции значений по кросс-ссылкам */

                 char   nCur_path[FILENAME_MAX] ;     /* Текущий раздел ОС */
//                int   nCnum_flag ;                  /* Флаг литерально-числового режима обработки */
                  int   nIndex_flag ;                 /* Флаг переменной с индексом */
                  int   nFragm_flag ;                 /* Флаг переменной с фрагментарным индексом */

                                                                    /* Файл DCL_INT.C */
                  int   Interpritator  (int, char *, Dcl_decl **) ;  /* DCL-интерпритатор */
                                       
                  int   Stack          (int) ;                       /* Стекование параметров интерпритатора */
#define                  _DCL_POP           -1                      /*  Извлечь из стека  */
#define                  _DCL_PUSH           1                      /*  Занести в стек    */
#define                  _DCL_CREATE_STACK   2                      /*  Создать стек      */
#define                  _DCL_DELETE_STACK  -2                      /*  Уничтожить стек   */
#define                  _DCL_CLEAR_STACK    0                      /*  Очистить стек     */
                  int   iSysin         (char *, long, int) ;         /* П/п ввода программы */
                 char  *iLeet          (char *) ;                    /* Обраб.оператора присвоения */
                  int   iIef_close     (int) ;                       /* Закрытие IF, ELSE и FOR операторов */

                                                                    /* Файл DCL_CULC.C */
                 char  *iCulculate     (char *) ;                    /* Вычисление выражения */
                 char  *iSimple_string (char *) ;                    /* Обработка простых строк */
                  int   iProcessor     (int, Dcl_decl *,             /* Процессор */
			                     Dcl_decl * ) ;
             Dcl_decl  *iComplex_var   (char *, char *, Dcl_decl *); /* Обр. произвольной переменной или процедуры */
             Dcl_decl  *iFind_var      (char *, int) ;               /* Поиск переменной или процедуры по имени */
                 char  *iFind_index    (Dcl_decl *, Dcl_decl *) ;    /* Поиск индексной строки в базовой строке */
                  int   iCulc_stack    (int) ;                       /* Стекование параметров процессора */

                                                                    /* Файл DCL_CMMN.C */
                 char  *iFind_close    (char *, int) ;               /* Поиск конечн.символа не включ. в скобки */
                 char  *iFind_open     (char *, char *, int) ;       /* Поиск начальн.символа не включ. в скобки */
#define                  _ANY_OPER          0
#define                  _ANY_OPER_OR_END   1
#define                  _ANY_CLOSE         2
                  int   iDgt_size      (int) ;                       /* Определение длины числового операнда */
               double   iDgt_get       (void *, int) ;               /* Извлечение числового значения */
               double   iDgt_set       (double, void *, int) ;       /* Занесение числового значения */
                 void   iSize_correct  (Dcl_decl *) ;                /* Следящая коррекция строчных обьектов */
                 void   iSize_correct  (Dcl_decl *, Dcl_decl *) ;
                 void   iIncl_correct  (Dcl_decl *, char *,          /* Следящая коррекция строчных обьектов при операции INCLUDE */
			                            char *, int) ;
                 void   iBuff_realloc  (Dcl_decl *,                  /* Переразмещение динамического массива */
                                        Dcl_decl *, size_t) ;
               double   iStrToNum      (char *, char **) ;           /* Перевод символьной строки в число */
                 void   iNumToStr      (Dcl_decl *, char *) ;        /* Перевод числа в символьную строку */
                 void   iLog           (char *, char *) ;            /* Вывод информации в отладочный лог */

                                                                    /* Файл DCL_XOBJ.C */
                  int   iXobject_check (char *, char *, char **) ;   /* Проверка на наличие операций "комплексирования" */
                  int   iXobject_clear (Dcl_decl *) ;                /* Очистка ресурсов объекта комплексного типа */
                  int   iXobject_add   (Dcl_decl *,                  /* Добавление записи для объекта комплексного типа */
                                        Dcl_complex_type *) ;
                  int   iXobject_index (Dcl_decl *, int *,           /* Индексирование объекта комплексного типа */
                                        Dcl_decl *        ) ;
                  int   iXobject_set   (Dcl_decl *,                  /* Пропись значений текущей записи для объекта комплексного типа */
                                        Dcl_decl *, int) ;
                  int   iXobject_delete(Dcl_decl *) ;                /* Удаление записей записей объекта комплексного типа */
                  int   iXobject_extend(Dcl_decl *, Dcl_decl *) ;    /* Добавление поля в структуру записей объекта комплексного типа */
                  int   iXobject_sort  (Dcl_decl *, char *) ;        /* Сортировка записей объекта комплексного типа по заданному полю */
                  int   iXobject_diff  (Dcl_decl *, Dcl_decl *,      /* Сравнение наборов записей объектов комплексного типа */
                                        Dcl_decl *, char *, char *) ;
             Dcl_decl  *iXobject_field (Dcl_complex_record *,        /* Операции над полями записи объекта комплексного типа */
                                                      char *, int) ;
             Dcl_decl  *iXobject_method(char *, char *) ;            /* Разборка вызова метода объекта комплексного типа */
             Dcl_decl  *iXobject_invoke(Dcl_decl *, Dcl_decl *,      /* Исполнение метода объекта комплексного типа */
                                        Dcl_decl **,  int       ) ;

                                                                    /* Файл DCL_DECL.C */
                  int   iDecl_detect   (char *) ;                    /* Обнаружение описаний переменных */
                  int   iDecl_maker    (char *, Dcl_decl *) ;        /* Обработка описаний переменных */
                  int   iType_define   (int, int, int, int) ;        /* Определение типа переменной */
                 void  *iTest_var      (char *, int) ;               /* Проверка наличия переменной */
#define                  _DCL_INTERNAL  0
#define                  _DCL_EXTERNAL  1
                                                                    /* Файл DCL_CALL.C */
		 void   iCall_init     (void) ;                      /* Инициализация системы подгрузки DCL-процедуры */
		 void   iCall_free     (void) ;                      /* Освобождение ресурсов системы подгрузки DCL-процедуры */
             Dcl_decl  *iCall_find     (char *) ;                    /* Поиск DCL-процедуры */
             Dcl_decl  *iCall_exec     (int) ;                       /* Исполнение DCL-процедуры */
             Dcl_call  *iCall_file     (char *, char *) ;            /* Поиск DCL-процедуры в файлах */
             Dcl_call  *iCall_lib      (char *, char *) ;            /* Поиск DCL-процедуры в библиотеках */
                 void   iCall_proc_list(Dcl_call *) ;                /* Составление списка локальных и виртуальных процедур */
                 void   iCall_final    (void) ;                      /* Исполнение процедур финального освобождения ресурсов */

                                                                    /* Файл DCL_MEM.C */
             Dcl_decl **iMem_init      (Dcl_decl **) ;               /* Инициализация системы управления памятью */
                 void   iMem_free      (void) ;                      /* Закрытие системы управления памятью */
             Dcl_decl  *iNext_title    (int) ;                       /* Выдача адреса свободного заголовка */
                 void  *iAlloc_var     (int, Dcl_decl *, int) ;      /* Размещение переменной в динамической памяти */
#define                        _WORK_VAR   1                         /* Рабочая переменная */
#define                    _INTERNAL_VAR   2                         /* Внутренняя переменная */
//#define                     _BLOCK_VAR   3                         /* Обьект типа BLOCK */
#define                  _CLEAR_WORK_VAR   4                         /* Освободить рабочие переменные */
              Dcl_decl *iGet_work      (int) ;                       /* Выдача заголовка рабочей ячейки по ее номеру */
                   int  iMem_stack     (int) ;                       /* Стек параметров управления памятью */

                                                                    /* Файл DCL_Z_FILELIST.C */
                   int  zGetFilesByMask  (Dcl_decl *, char *) ;      /* Построение списка файлов по маске */
                   int  zGetFilesByFolder(Dcl_decl *, char *) ;      /* Построение списка файлов по маске */
                  char *zGetMaskFiles    (char *) ;                  /* Получение списка объектов раздела */
                   int  zTestMask        (char *, char *) ;          /* Проверка фрагмента по маске */

                                                                    /* Файл DCL_Z_SQL.C */
                   int  zSqlSelectOnce (Dcl_decl *,                  /* Разовое выполнение SELECT-выборки */
                                        Dcl_decl **, int) ; 
                   int  zSqlDmlExecute (Dcl_decl *,                  /* Выполнение DML-оператора по всем элементам объекта */
                                        Dcl_decl **, int) ; 
                   int  zSqlDescribe   (Dcl_decl *,                  /* Получение описание табличного объекта */
                                        Dcl_decl **, int) ; 
                   int  zDbfRead       (Dcl_decl *,                  /* Считывание DBF-файла */
                                        Dcl_decl **, int) ; 

                                                                    /* Файл DCL_Z_MIDAS.C */
                   int  zMidasApiNew   (Dcl_decl *,                  /* Создание шаблона сообщения MIDAS API */
                                        Dcl_decl **, int) ; 
                   int  zMidasApiSend  (Dcl_decl *,                  /* Отправка сообщения MIDAS API */
                                        Dcl_decl **, int) ; 
                   int  zMidasApiErrors(Dcl_decl *,                  /* Извлечение ошибок MIDAS API */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* Файл DCL_Z_QC.C */
                   int  zQC_ResultNew  (Dcl_decl *,                  /* Добавление новой записи QC-result */
                                        Dcl_decl **, int) ; 
                   int  zQC_ResultSend (Dcl_decl *,                  /* Обработка записей QC-result */
                                        Dcl_decl **, int) ; 
                   int  zQC_Errors     (Dcl_decl *,                  /* Извлечение ошибок работы с QC */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* Файл DCL_Z_EMAIL.C */
                   int  zEMailNew      (Dcl_decl *,                  /* Добавление новой записи EMAIL */
                                        Dcl_decl **, int) ; 
                   int  zEMailSend     (Dcl_decl *,                  /* Отправка Email-сообщений */
                                        Dcl_decl **, int) ; 
                   int  zEMailErrors   (Dcl_decl *,                  /* Извлечение ошибок работы с EMAIL */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* Файл DCL_Z_ABTP.C */
                   int  zAbtpSignal    (char *, char *, char *,      /* Отправка сообщения ABTP:SIGNAL */
                                                char *,  int   ) ;

                                                                    /* Файл DCL_Z_STRING.C */
                   int  zParseByTemplate(char *, int, char *, int,   /* Разбор текста по шаблону */
                                                      char *, int,
                                                  Dcl_decl *, int ) ;
                   int  zFormByTemplate (char *, int, char *, int,   /* Формирование текста по шаблону */
                                                      char *, int,
                                                  Dcl_decl *, int ) ;
                   int  zCodePageConvert(char *, int,                /* Преобразование кодовой страницы */
                                                char *, char *) ;
                  void  zDosToWin       (char *, long) ;             /* Преобразование кодовой страницы DOS->WIN */
                  void  zWinToDos       (char *, long) ;             /* Преобразование кодовой страницы WIN->DOS */
                  long  zUTF8ToWin      (char *, long, char *) ;     /* Преобразование кодовой страницы UTF8->WIN */
                  long  zWinToUTF8      (char *, long, char *) ;     /* Преобразование кодовой страницы WIN->UTF8 */

                                   } ;

/*----------------------------------- Описание переменных и процедур */

#define  _DGT_VAL     _DCLT_DGT_VAL     /* Число */
#define  _DGT_AREA    _DCLT_DGT_AREA    /* Массив чисел или указатель на число */
#define  _DGT_PTR     _DCLT_DGT_PTR     /* Указатель на массив чисел */
#define  _CHR_AREA    _DCLT_CHR_AREA    /* Строка(массив символов) */
#define  _CHR_PTR     _DCLT_CHR_PTR     /* Указатель на строку */
#define  _XTP_OBJ     _DCLT_XTP_OBJ     /* Объект комплексного типа */
#define  _XTP_REC     _DCLT_XTP_REC     /* Запись объекта комплексного типа */
#define  _NULL_PTR    _DCLT_NULL_PTR    /* NULL - указатель */

#define  t_base(a)   (a & _DCLT_BASE_MASK)
#define  t_mode(a)   (a & _DCLT_MODE_MASK)

/* Описатели параметров функции */
#define  _F_ANY_TYPE   'a'   /* Произвольный тип */
#define  _F_DGT_VAL    'v'   /* Число */
#define  _F_DGT_PTR    'd'   /* Указатель на массив чисел */
#define  _F_CHR_PTR    's'   /* Указатель на строку */

/*------------------------------------------------- Задающие команды */

#define  _DCL_MEMORY   0
#define  _DCL_FILE     1
#define  _DCL_INIT     2

/*----------------------------------------------------------- Ошибки */

#define  _DCLE_FILE_OPEN       2    /* Ошибка открытия файла */
#define  _DCLE_STRING_LEN      3    /* Слишком длинная строка */
#define  _DCLE_NOCLOSE_STRING  4    /* Незакрытые кавычки */
#define  _DCLE_MARK_SIMBOL     5    /* Некорректная метка */
#define  _DCLE_MARK_LEN        6    /* Слишком длинная метка */
#define  _DCLE_IF_CLOSE        7    /* Потеряна закр.скобка оператора IF */
#define  _DCLE_FOR_CLOSE       8    /* Потеряна закр.скобка заголовка цикла */
#define  _DCLE_RETURN_CLOSE    9    /* Потеряна закр.скобка оператора RETURN */
#define  _DCLE_FOR_TTL_SEL    10    /* Потерян рассекатель частей заголовка */
#define  _DCLE_FREE_OPEN      11    /* Открытие 'свободного' блока */
#define  _DCLE_FREE_CLOSE     12    /* Закрытие неоткрытого блока */
#define  _DCLE_FREE_CONTINUE  14    /* 'Свободный' оператор CONTINUE */
#define  _DCLE_FREE_BREAK     15    /* 'Свободный' оператор BREAK */
#define  _DCLE_FREE_ELSE      16    /* ELSE без IF-а */
#define  _DCLE_OPEN           17    /* Незакрытое выражение в скобках */
#define  _DCLE_CLOSE          18    /* Лишняя закрывающая скобка */
#define  _DCLE_UNKNOWN_NAME   19    /* Неизвестная переменная или процедура */
#define  _DCLE_SINTAX_NAME    20    /* Некорректный синтакс имени */
#define  _DCLE_NOINDEX        21    /* Пустое или незакрытое индексное выражение */
#define  _DCLE_INDEXED_VAR    22    /* Индексирование простой переменной */
#define  _DCLE_INDEX_TYPE     23    /* Некорректный тип индекса */
#define  _DCLE_INDEX_FRG      24    /* Фрагментарное индексирование нестрочного объекта */
#define  _DCLE_INVALID_CHR    25    /* Некорректный символ в символьной константе */
#define  _DCLE_BAD_OPERATION  26    /* Некорректная операция */
#define  _DCLE_BAD_LEFT       27    /* Некорректный левый операнд */
#define  _DCLE_BAD_RIGHT      28    /* Некорректный правый операнд */
#define  _DCLE_EMPTY_LEFT     29    /* Отсутствует левый операнд */
#define  _DCLE_EMPTY_RIGHT    30    /* Отсутствует правый операнд */
#define  _DCLE_EMPTY_BOTH     31    /* Операция без операндов */
#define  _DCLE_BINARY_OPER    32    /* Бинарная операция с одним операндом */
#define  _DCLE_UNARY_OPER     33    /* Унарная операция с двумя операндами */
#define  _DCLE_PREFIX_OPER    34    /* Операция должна быть префиксной */
#define  _DCLE_EMPTY_DEST     35    /* При операции отсутствует 'приемник' */
#define  _DCLE_DIF_TYPE       36    /* Тип 'приемника' не соотв.типу результата */
#define  _DCLE_CONST_DEST     37    /* 'Приемник' является немодифицируемым */
#define  _DCLE_PROTOTYPE      38    /* Тип параметра не соответствует прототипу */
#define  _DCLE_TYPEDEF_ELEM   39    /* Неизвестный элемент комплексного объекта */
#define  _DCLE_TYPEDEF_METH   40    /* Неизвестный метод комплексного объекта */
#define  _DCLE_TYPEDEF_SORT   41    /* Поле сортировки имеет различный или не поддерживаемый тип */
#define  _DCLE_TYPEDEF_AREA   42    /* Динамический массив в комплексном обьекте */
#define  _DCLE_TBLOCK_CLOSE   43    /* Не закрытый текстовой блок */
#define  _DCLE_TBLOCK_SIZE    44    /* Слишком большой текстовой блок */
#define  _DCLE_READONLY       45    /* Попытка изменения READONLY-переменной */

#define  _DCLE_PRAGMA_UNK    201    /* Неизвестная прагма */

#define  _DCLE_BAD_PREF      401    /* Некорректный префикс */
#define  _DCLE_DBL_MOD       402    /* Некорректное описание */
#define  _DCLE_NO_TYPE       403    /* Описание не задает базовый тип */
#define  _DCLE_PTR_AREA      404    /* Массив указателей запрещен */
#define  _DCLE_BLOCK_DECL    405    /* Обьект BLOCK должен быть простым массивом */
#define  _DCLE_TYPEDEF_IN    406    /* Вложенный оператор typedef */
#define  _DCLE_TYPEDEF_STR   407    /* Некорректная структура оператора typedef */
#define  _DCLE_TYPEDEF_RDF   408    /* Переопределение типа */
#define  _DCLE_TYPEDEF_NAME  409    /* Имя типа не соответствует соглашениям */
#define  _DCLE_NAME_INUSE    410    /* Имя переменной уже используется */

#define  _DCLE_CALL_INSIDE   501    /* Внутри-процедурная ошибка */

#define  _DCLE_USER_DEFINED  601    /* Ошибка контекста исполнения */

#define  _DCLE_ELIB_INIT     701    /* Ошибка иниц.системы внешних библиотек */
#define  _DCLE_ELIB_DECL     702    /* Некорректное описание обьекта во внешней библиотеке */
#define  _DCLE_ELIB_LOAD     703    /* Неудачная загрузка обьекта из внешней библиотеке */

#define  _DCLE_MEMORY        801    /* Недостаток памяти */
#define  _DCLE_MEMORY_VT     802    /* Недостаток памяти для заголовков переменных */
#define  _DCLE_MEMORY_VA     804    /* Недостаток памяти для переменных */
#define  _DCLE_LOAD_LIST     808    /* Слишко много подгружаемых переменных */

#define  _DCLE_STACK_OVR     809    /* Переполнение стека */
#define  _DCLE_STACK_UND     810    /* Извлечение пустого стека */
#define  _DCLE_STACK_MEM     811    /* Недостаточно памяти для стека */

#define  _DCLE_UNKNOWN       901    /* Имени нет в списках */
#define  _DCLE_EMPTY         902    /* Пустое простое выражение */
#define  _DCLE_WORK_FAULT    903    /* Сбой адресации заголовка рабочей ячейки */

/*--------------------------- Описание символьного состава элементов */

#define  ALFANUM_CHAR(c)  (isalnum(c) || (c>='А' && c<='Э') \
				      || (c>='а' && c<='п') \
				      || (c>='р' && c<='э'))

#define   DIGITS_CHAR(c)  (isdigit(c) || c=='+' || c=='-' \
				      || c=='e' || c=='E' \
				      || c=='.'          )

#define     NAME_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
						|| c=='#')

#define      VAR_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
				      || c=='[' || c==']' \
				      || c=='(' || c==')' \
				      || c==',' || c=='#')

#define     SPEC_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
				      || c=='[' || c==']' \
				      || c=='(' || c==')' \
				      || c==',' || c==':' \
				      || c=='+' || c=='-' \
				      || c=='#' || c=='.')

#define     OPER_CHAR(c)             (   c=='+' || c=='-' \
				      || c=='*' || c=='/' \
				      || c=='^' || c=='!' \
				      || c=='=' || c=='>' \
				      || c=='<' || c=='&' \
				      || c=='|' || c=='@' )

/*----------------------------------------------- Связные переменные */

// extern Dcl_decl  dcl_debug_lib[] ;  /* Библиотека утилит отладки */
// extern Dcl_decl  dcl_std_lib[] ;    /* Системная библиотека */
// extern Dcl_decl  dcl_file_lib[] ;   /* Библиотека работы с файлами */
// extern Dcl_decl  dcl_sql_lib[] ;    /* Библиотека работы с SQL-запросами */

/*-------------------------------------------------------------------*/

#endif
